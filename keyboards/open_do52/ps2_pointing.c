/*
 * Custom pointing device driver wrapping PS/2 interrupt for split forwarding.
 *
 * QMK's built-in PS2_MOUSE doesn't integrate with the pointing device
 * framework, so SPLIT_POINTING_ENABLE can't forward data between halves.
 * This driver bridges PS/2 ↔ pointing device to enable:
 *
 *   Right half USB host: PS/2 → pointing device → USB (direct)
 *   Left half USB host:  PS/2 → pointing device → serial → USB
 *
 * PS/2 is initialized only on the right half (where the TrackPoint lives).
 * We use remote mode and poll explicitly, throttled to ~66Hz so the
 * split serial transport gets enough CPU time on the slave half.
 */

#include "quantum.h"
#include "pointing_device.h"
#include "ps2.h"
#include "ps2_mouse.h"
#include "wait.h"
#include "print.h"
#include "timer.h"

static bool ps2_initialized = false;

bool pointing_device_driver_init(void) {
    /* Only initialize PS/2 on the right half (where the TrackPoint is wired) */
    if (!is_keyboard_left()) {
        ps2_host_init();
        wait_ms(PS2_MOUSE_INIT_DELAY);

        /* Reset mouse */
        PS2_MOUSE_SEND(PS2_MOUSE_RESET, "tp: reset");
        PS2_MOUSE_RECEIVE("tp: BAT");
        PS2_MOUSE_RECEIVE("tp: DevID");

        /* Remote mode — we poll explicitly in get_report */
        PS2_MOUSE_SEND(PS2_MOUSE_SET_REMOTE_MODE, "tp: remote mode");

        ps2_initialized = true;
    }
    return true;
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    if (!ps2_initialized) {
        return mouse_report;
    }

    /* Throttle polling to ~66Hz so split serial transport has CPU time */
    static uint16_t last_poll = 0;
    if (timer_elapsed(last_poll) < 15) {
        return mouse_report;
    }
    last_poll = timer_read();

    /* Poll TrackPoint (remote mode: send READ_DATA, expect ACK + 3 bytes) */
    uint8_t rcv = ps2_host_send(PS2_MOUSE_READ_DATA);
    if (rcv != PS2_ACK) {
        return mouse_report;
    }

    /* Read 3-byte PS/2 mouse packet */
    uint8_t buttons_raw = ps2_host_recv_response();
    int8_t  x_raw       = (int8_t)ps2_host_recv_response();
    int8_t  y_raw       = (int8_t)ps2_host_recv_response();

    /* Sign-extend and clamp to [-127, 127] using PS/2 9-bit sign bits */
    bool x_neg = buttons_raw & (1 << PS2_MOUSE_X_SIGN);
    bool x_ovf = buttons_raw & (1 << PS2_MOUSE_X_OVFLW);
    bool y_neg = buttons_raw & (1 << PS2_MOUSE_Y_SIGN);
    bool y_ovf = buttons_raw & (1 << PS2_MOUSE_Y_OVFLW);

    int8_t x = x_neg ? ((!x_ovf && x_raw >= -127 && x_raw <= -1) ? x_raw : -127)
                      : ((!x_ovf && x_raw >= 0 && x_raw <= 127) ? x_raw : 127);
    int8_t y = y_neg ? ((!y_ovf && y_raw >= -127 && y_raw <= -1) ? y_raw : -127)
                      : ((!y_ovf && y_raw >= 0 && y_raw <= 127) ? y_raw : 127);

    /* Invert Y (PS/2 Y-axis is inverted relative to USB HID) */
    y = -y;

    /* Rotate 90 degrees for TrackPoint orientation: x' = y, y' = -x */
    mouse_report.x       = y;
    mouse_report.y       = -x;
    mouse_report.buttons = buttons_raw & PS2_MOUSE_BTN_MASK;

    return mouse_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return 0;
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    /* TrackPoint sensitivity is handled via software scaling in open_do52.c */
}
