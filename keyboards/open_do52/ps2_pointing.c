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
 * Stream mode: the TrackPoint sends data automatically when moved.
 * The interrupt driver captures bytes via ISR into a ring buffer.
 * get_report() is completely non-blocking — it just drains the buffer.
 */

#include "quantum.h"
#include "pointing_device.h"
#include "ps2.h"
#include "ps2_mouse.h"
#include "wait.h"
#include "print.h"

static bool ps2_initialized = false;

bool pointing_device_driver_init(void) {
    /* Only initialize PS/2 on the right half (where the TrackPoint is wired) */
    if (!is_keyboard_left()) {
        ps2_host_init();
        wait_ms(PS2_MOUSE_INIT_DELAY);

        /* Reset mouse — device ACKs then runs self-test (300-500ms) */
        PS2_MOUSE_SEND(PS2_MOUSE_RESET, "tp: reset");

        /* Wait for self-test to finish.  recv_response() only waits
         * 25ms, but the self-test takes 300-500ms after a software
         * reset.  Without this delay the subsequent commands arrive
         * while the device is busy and get silently ignored — which
         * is fatal for stream mode (ENABLE_DATA_REPORTING never
         * takes effect, so the TrackPoint never sends data). */
        wait_ms(500);
        PS2_MOUSE_RECEIVE("tp: BAT");
        PS2_MOUSE_RECEIVE("tp: DevID");

        /* Stream mode — enable data reporting, then set stream mode
         * (matches QMK's proven init order in ps2_mouse.c) */
        PS2_MOUSE_SEND(PS2_MOUSE_ENABLE_DATA_REPORTING, "tp: enable");
        PS2_MOUSE_SEND(PS2_MOUSE_SET_STREAM_MODE, "tp: stream");

        /* Drain any stale bytes left from init handshake so they
         * don't corrupt the 3-byte packet alignment in stream mode. */
        while (pbuf_has_data()) {
            ps2_host_recv();
        }

        ps2_initialized = true;
    }
    return true;
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    if (!ps2_initialized) {
        return mouse_report;
    }

    /*
     * Stream mode: the TrackPoint sends 3-byte packets via interrupt.
     * Drain the ring buffer and use the latest complete packet.
     * Partial packets are held in the static buffer for the next call.
     */
    static uint8_t pkt[3];
    static uint8_t pkt_pos = 0;

    uint8_t latest_buttons = 0;
    int8_t  latest_x       = 0;
    int8_t  latest_y       = 0;
    bool    has_packet      = false;

    while (pbuf_has_data()) {
        uint8_t b = ps2_host_recv();
        if (ps2_error) break;

        /* PS/2 byte 0 always has bit 3 set — use this to re-sync
         * if we ever lose the 3-byte packet boundary. */
        if (pkt_pos == 0 && !(b & 0x08)) {
            continue;
        }

        pkt[pkt_pos++] = b;
        if (pkt_pos == 3) {
            pkt_pos = 0;
            latest_buttons = pkt[0];
            latest_x       = (int8_t)pkt[1];
            latest_y       = (int8_t)pkt[2];
            has_packet     = true;
        }
    }

    if (!has_packet) {
        return mouse_report;
    }

    /* Sign-extend and clamp to [-127, 127] using PS/2 9-bit sign bits */
    bool x_neg = latest_buttons & (1 << PS2_MOUSE_X_SIGN);
    bool x_ovf = latest_buttons & (1 << PS2_MOUSE_X_OVFLW);
    bool y_neg = latest_buttons & (1 << PS2_MOUSE_Y_SIGN);
    bool y_ovf = latest_buttons & (1 << PS2_MOUSE_Y_OVFLW);

    int8_t x = x_neg ? ((!x_ovf && latest_x >= -127 && latest_x <= -1) ? latest_x : -127)
                      : ((!x_ovf && latest_x >= 0 && latest_x <= 127) ? latest_x : 127);
    int8_t y = y_neg ? ((!y_ovf && latest_y >= -127 && latest_y <= -1) ? latest_y : -127)
                      : ((!y_ovf && latest_y >= 0 && latest_y <= 127) ? latest_y : 127);

    /* Invert Y (PS/2 Y-axis is inverted relative to USB HID) */
    y = -y;

    /* Rotate 90 degrees for TrackPoint orientation: x' = y, y' = -x */
    mouse_report.x       = y;
    mouse_report.y       = -x;
    mouse_report.buttons = latest_buttons & PS2_MOUSE_BTN_MASK;

    return mouse_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return 0;
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    /* TrackPoint sensitivity is handled via software scaling in open_do52.c */
}
