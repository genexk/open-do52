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

        /* Drain stale bytes from init, then wait for any in-flight
         * stream packet to finish and drain that too.  A partial
         * packet left in the buffer would permanently misalign the
         * 3-byte framing in get_report(). */
        while (pbuf_has_data()) ps2_host_recv();
        wait_ms(10);
        while (pbuf_has_data()) ps2_host_recv();

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
     * Drain the ring buffer and use the latest VALID packet.
     *
     * The bitbang serial driver disables global interrupts (cli/sei)
     * during every split transaction, which can cause PS/2 bytes to
     * be lost or corrupted.  A single lost byte permanently shifts
     * the 3-byte packet boundary, making movement data look like
     * button presses and vice-versa ("cursor flies everywhere").
     *
     * Defence: validate byte 0 (bit 3 set, overflow clear) AND
     * check that the sign bits in byte 0 are consistent with the
     * sign of the movement bytes.  This rejects ~99% of misaligned
     * or corrupted packets.
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

        if (pkt_pos == 0) {
            /* Byte 0: bit 3 must be set (PS/2 spec "always 1"),
             * and overflow bits (6-7) must be clear. */
            if ((b & 0xC8) != 0x08) {
                continue;
            }
        }

        pkt[pkt_pos++] = b;
        if (pkt_pos == 3) {
            pkt_pos = 0;

            int8_t x = (int8_t)pkt[1];
            int8_t y = (int8_t)pkt[2];

            /* Sign-consistency check: the sign bits in byte 0
             * must agree with the actual sign of bytes 1-2.
             * A misaligned packet almost never passes this. */
            bool x_sign = pkt[0] & (1 << PS2_MOUSE_X_SIGN);
            bool y_sign = pkt[0] & (1 << PS2_MOUSE_Y_SIGN);
            if ((x_sign && x > 0) || (!x_sign && x < 0)) continue;
            if ((y_sign && y > 0) || (!y_sign && y < 0)) continue;

            latest_buttons = pkt[0];
            latest_x       = x;
            latest_y       = y;
            has_packet     = true;
        }
    }

    if (!has_packet) {
        return mouse_report;
    }

    /* Invert Y (PS/2 Y+ is up, USB HID Y+ is down) and
     * rotate 90° for TrackPoint orientation: x' = y, y' = -x */
    mouse_report.x       = -latest_y;
    mouse_report.y       = -latest_x;
    mouse_report.buttons = latest_buttons & PS2_MOUSE_BTN_MASK;

    return mouse_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return 0;
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    /* TrackPoint sensitivity is handled via software scaling in open_do52.c */
}
