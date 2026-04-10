#include "open_do52.h"
#include "ps2.h"

/* ---- EEPROM user config ---- */

typedef union {
    uint32_t raw;
    struct {
        uint8_t tp_sensitivity;
        uint8_t _padding[3];
    };
} user_config_t;

static user_config_t user_config;

/* ---- TrackPoint sensitivity via PS/2 ---- */

static void tp_set_sensitivity(uint8_t sensitivity) {
    if (is_keyboard_left()) {
        return; /* TrackPoint is on the right half only */
    }
    /* TrackPoint PS/2 sensitivity command:
     * Send 0xE2 (vendor-specific set), then 0x4A (sensitivity register), then value */
    ps2_host_send(0xE2);
    ps2_host_send(0x4A);
    ps2_host_send(sensitivity);
}

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
    tp_set_sensitivity(user_config.tp_sensitivity);
    keyboard_post_init_user();
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
                tp_set_sensitivity(user_config.tp_sensitivity);
                eeconfig_update_kb(user_config.raw);
                return false;
            case QK_KB_1: /* SENS_DN */
                if (user_config.tp_sensitivity >= TP_SENSITIVITY_STEP) {
                    user_config.tp_sensitivity -= TP_SENSITIVITY_STEP;
                } else {
                    user_config.tp_sensitivity = 1;
                }
                tp_set_sensitivity(user_config.tp_sensitivity);
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
                tp_set_sensitivity(user_config.tp_sensitivity);
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
