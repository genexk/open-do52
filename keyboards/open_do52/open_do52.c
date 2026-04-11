#include "open_do52.h"
#include "pointing_device.h"

/* ---- EEPROM user config ---- */

typedef union {
    uint32_t raw;
    struct {
        uint8_t tp_sensitivity;
        uint8_t _padding[3];
    };
} user_config_t;

static user_config_t user_config;

/* ---- Keyboard init ---- */

void eeconfig_init_kb(void) {
    user_config.tp_sensitivity = TP_SENSITIVITY_DEFAULT;
    eeconfig_update_kb(user_config.raw);
    eeconfig_init_user();
}

void keyboard_post_init_kb(void) {
    user_config.raw = eeconfig_read_kb();
    if (user_config.tp_sensitivity == 0 || user_config.tp_sensitivity == 0xFF) {
        user_config.tp_sensitivity = TP_SENSITIVITY_DEFAULT;
        eeconfig_update_kb(user_config.raw);
    }
    keyboard_post_init_user();
}

/* ---- Software sensitivity scaling ---- */
/*
 * Runs on the master (USB) side after the report arrives from either
 * the local driver or the slave half via split transport.
 * This ensures VIA slider changes take effect immediately regardless
 * of which side has USB connected.
 */
report_mouse_t pointing_device_task_kb(report_mouse_t mouse_report) {
    if (user_config.tp_sensitivity != 128) {
        int16_t x = ((int16_t)mouse_report.x * user_config.tp_sensitivity) / 128;
        int16_t y = ((int16_t)mouse_report.y * user_config.tp_sensitivity) / 128;
        mouse_report.x = (x > 127) ? 127 : (x < -127) ? -127 : (int8_t)x;
        mouse_report.y = (y > 127) ? 127 : (y < -127) ? -127 : (int8_t)y;
    }
    return pointing_device_task_user(mouse_report);
}

/* ---- DPAD Mode ---- */

/*
 * DPAD matrix positions (per side):
 * Row 4 (left) / Row 10 (right), columns 0-5
 *
 * Mode 0 (default): all 6 positions active, mapped in keymap
 * Mode 1 (single key): only center (col 2) active, rest blocked
 * Mode 2 (encoder): encoder mapped to matrix via QMK encoder driver
 */

#if DPAD_MODE == 1
static bool process_record_kb_dpad(uint16_t keycode, keyrecord_t *record) {
    uint8_t row = record->event.key.row;
    uint8_t col = record->event.key.col;
    if (row == 4 || row == 10) {
        if (col != 2) {
            return false; /* Block non-center keys */
        }
    }
    return true;
}
#endif

/* ---- Custom keycodes (SENS_UP / SENS_DN) ---- */

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
#if DPAD_MODE == 1
    if (!process_record_kb_dpad(keycode, record)) {
        return false;
    }
#endif
    if (!process_record_user(keycode, record)) {
        return false;
    }
    if (record->event.pressed) {
        switch (keycode) {
            case QK_KB_0: /* SENS_UP */
                if (user_config.tp_sensitivity <= 255 - TP_SENSITIVITY_STEP) {
                    user_config.tp_sensitivity += TP_SENSITIVITY_STEP;
                } else {
                    user_config.tp_sensitivity = 255;
                }
                eeconfig_update_kb(user_config.raw);
                return false;
            case QK_KB_1: /* SENS_DN */
                if (user_config.tp_sensitivity >= TP_SENSITIVITY_STEP) {
                    user_config.tp_sensitivity -= TP_SENSITIVITY_STEP;
                } else {
                    user_config.tp_sensitivity = 1;
                }
                eeconfig_update_kb(user_config.raw);
                return false;
        }
    }
    return true;
}

/* ---- VIA custom value channel ---- */

#ifdef VIA_ENABLE
#include "via.h"

enum via_custom_channel {
    id_tp_sensitivity = 1
};

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id = &data[0];
    uint8_t *channel_id = &data[1];
    uint8_t *value_id   = &data[2];

    if (*channel_id != id_custom_channel) {
        return;
    }

    switch (*command_id) {
        case id_custom_set_value:
            if (*value_id == id_tp_sensitivity) {
                user_config.tp_sensitivity = data[3];
                eeconfig_update_kb(user_config.raw);
            }
            break;
        case id_custom_get_value:
            if (*value_id == id_tp_sensitivity) {
                data[3] = user_config.tp_sensitivity;
            }
            break;
        case id_custom_save:
            eeconfig_update_kb(user_config.raw);
            break;
    }
}
#endif /* VIA_ENABLE */
