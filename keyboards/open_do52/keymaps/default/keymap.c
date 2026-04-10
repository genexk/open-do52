#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /*
     * Layer 0: Base QWERTY
     *
     * Left:                                    Right:
     * TAB   Q     W     E     R     T          Y     U     I     O     P     BSLS
     * CAPS  A     S     D     F     G          H     J     K     L     ;     '
     * LSFT  Z     X     C     V     B          N     M     ,     .     /     RSFT
     * LCTL  MO(2) UP    DOWN  LALT  SPC        SPC   MO(1) SPC   SPC   RCTL  ENT
     * LEFT  UP    .     DOWN  RGHT  LEFT       NO    BTN2  BTN1  NO    NO    6
     * ESC   1     2     3     4     5          6     7     8     9     0     BSPC
     */
    [0] = LAYOUT(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,         KC_Y,       KC_U,       KC_I,       KC_O,    KC_P,    KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,         KC_H,       KC_J,       KC_K,       KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,         KC_N,       KC_M,       KC_COMM,    KC_DOT,  KC_SLSH, KC_RSFT,
        KC_LCTL, MO(2),   KC_UP,   KC_DOWN, KC_LALT, KC_SPC,       KC_SPC,     MO(1),      KC_SPC,     KC_SPC,  KC_RCTL, KC_ENT,
        KC_LEFT, KC_UP,   KC_NO,   KC_DOWN, KC_RGHT, KC_LEFT,      KC_NO,      MS_BTN2, MS_BTN1, KC_NO,   KC_NO,   KC_6,
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,         KC_6,       KC_7,       KC_8,       KC_9,    KC_0,    KC_BSPC
    ),

    /*
     * Layer 1: Navigation + Symbols (MO(1) — right thumb)
     *
     * Left:                                    Right:
     * ___   HOME  UP    END   PGUP  ___        ___   ___   ___   ___   -     =
     * ___   LEFT  DOWN  RGHT  PGDN  ___        ___   ___   ___   ___   [     ]
     * ___   ___   ___   ___   ___   ___        BTN1  BTN2  ___   ___   ___   ___
     * ___   ___   ___   ___   ___   ___        ___   ___   ___   ___   ___   ___
     * ___   ___   ___   ___   ___   ___        ___   ___   ___   ___   ___   ___
     * ___   `     ___   ___   ___   ___        ___   ___   ___   ___   DEL   ___
     */
    [1] = LAYOUT(
        KC_TRNS, KC_HOME, KC_UP,   KC_END,  KC_PGUP, KC_TRNS,      KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_MINS, KC_EQL,
        KC_TRNS, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_TRNS,      KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      MS_BTN1, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_GRV,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS,    KC_TRNS,    KC_TRNS, KC_TRNS, KC_DEL,  KC_TRNS
    ),

    /*
     * Layer 2: Shifted Symbols (MO(2) — left thumb)
     *
     * Left:                                    Right:
     * ___   ___   ___   ___   ___   ___        ___   ___   ___   ___   _     +
     * ___   ___   ___   ___   ___   ___        ___   ___   ___   ___   {     }
     * ___   ___   ___   ___   ___   ___        ___   ___   ___   ___   ___   ___
     * ___   ___   ___   ___   ___   ___        ___   ___   ___   ___   ___   ___
     * ___   ___   ___   ___   ___   ___        ___   ___   ___   ___   ___   ___
     * ___   ~     ___   ___   ___   ___        ___   ___   ___   ___   ___   ___
     */
    [2] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, S(KC_MINS), S(KC_EQL),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, S(KC_LBRC), S(KC_RBRC),
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS,
        KC_TRNS, S(KC_GRV), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS
    ),

    /*
     * Layer 3: Reserved (empty)
     */
    [3] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};
