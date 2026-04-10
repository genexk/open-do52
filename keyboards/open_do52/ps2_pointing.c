/*
 * Custom pointing device driver that wraps QMK's PS/2 mouse.
 *
 * QMK's PS/2 mouse support is standalone and doesn't integrate with the
 * pointing device framework. This driver bridges the two so that
 * SPLIT_POINTING_ENABLE can forward trackpoint data between halves.
 *
 * When the right half is USB host: PS/2 → pointing device → USB (direct)
 * When the left half is USB host:  PS/2 → pointing device → serial → USB
 */

#include "quantum.h"
#include "pointing_device.h"
#include "ps2_mouse.h"
#include "ps2.h"

static bool ps2_initialized = false;

void pointing_device_driver_init(void) {
    /* Only initialize PS/2 on the right half (where the TrackPoint is wired) */
    if (!is_keyboard_left()) {
        ps2_mouse_init();
        ps2_initialized = true;
    }
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    if (!ps2_initialized) {
        return mouse_report;
    }

    /* Read PS/2 data — ps2_mouse_read_report fills the global ps2_mouse_report */
    uint8_t data[PS2_MOUSE_READ_SIZE];
    if (ps2_host_recv() == -1) {
        return mouse_report;
    }

    /* Use PS/2 low-level read to get movement data */
    data[0] = ps2_host_recv();
    if (data[0] == -1) {
        return mouse_report;
    }
    data[1] = ps2_host_recv();
    data[2] = ps2_host_recv();

    /* Parse PS/2 mouse packet */
    /* Byte 0: Y overflow | X overflow | Y sign | X sign | 1 | Middle | Right | Left */
    /* Byte 1: X movement */
    /* Byte 2: Y movement */

    uint8_t buttons = data[0] & 0x07;
    int16_t x = (data[0] & (1 << 4)) ? (int16_t)data[1] - 256 : data[1];
    int16_t y = (data[0] & (1 << 5)) ? (int16_t)data[2] - 256 : data[2];

    /* Apply rotation (270 degrees: x' = y, y' = -x) */
    int16_t rotated_x = y;
    int16_t rotated_y = -x;

    mouse_report.x = (int8_t)(rotated_x > 127 ? 127 : (rotated_x < -127 ? -127 : rotated_x));
    mouse_report.y = (int8_t)(rotated_y > 127 ? 127 : (rotated_y < -127 ? -127 : rotated_y));
    mouse_report.buttons = buttons;

    return mouse_report;
}

uint16_t pointing_device_driver_get_cpi(void) {
    return 0; /* CPI not directly applicable to PS/2 TrackPoint */
}

void pointing_device_driver_set_cpi(uint16_t cpi) {
    /* TrackPoint sensitivity is handled via PS/2 0xE2 command in open_do52.c */
}
