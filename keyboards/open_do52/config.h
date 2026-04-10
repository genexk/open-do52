#pragma once

/* Split keyboard — EE_HANDS */
#define EE_HANDS
#define SPLIT_USB_DETECT

/* PS/2 TrackPoint (right half, busywait driver) */
#define PS2_CLOCK_PIN B1
#define PS2_DATA_PIN  B2

/* Pointing device on the right half */
#define POINTING_DEVICE_RIGHT
#define SPLIT_POINTING_ENABLE

/* TrackPoint tuning — remote mode and 270° rotation handled in ps2_pointing.c */

/* TrackPoint sensitivity — default value (0-255), stored in EEPROM */
#define TP_SENSITIVITY_DEFAULT 128
#define TP_SENSITIVITY_STEP    16

/* DPAD mode: 0 = 5-way nav (default), 1 = single key, 2 = rotary encoder */
#ifndef DPAD_MODE
#    define DPAD_MODE 0
#endif

/* Encoder config (only used when DPAD_MODE == 2)
 * Encoder A/B wired to unused pins B3, B6 (one encoder per half).
 * Update these if your wiring differs. */
#if DPAD_MODE == 2
#    define ENCODER_A_PINS { B3 }
#    define ENCODER_B_PINS { B6 }
#    define ENCODER_A_PINS_RIGHT { B3 }
#    define ENCODER_B_PINS_RIGHT { B6 }
#    define ENCODER_RESOLUTION 4
#endif

/* EEPROM for user config (sensitivity) */
#define EECONFIG_USER_DATA_SIZE 4
