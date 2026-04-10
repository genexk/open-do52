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

/* TrackPoint tuning */
#define PS2_MOUSE_USE_REMOTE_MODE
#define PS2_MOUSE_ROTATE 270

/* TrackPoint sensitivity — default value (0-255), stored in EEPROM */
#define TP_SENSITIVITY_DEFAULT 128
#define TP_SENSITIVITY_STEP    16

/* DPAD mode: 0 = 5-way nav (default), 1 = single key, 2 = rotary encoder */
#ifndef DPAD_MODE
#    define DPAD_MODE 0
#endif

/* Encoder config (only used when DPAD_MODE == 2) */
#if DPAD_MODE == 2
#    define ENCODER_RESOLUTION 4
#endif

/* EEPROM for user config (sensitivity) */
#define EECONFIG_USER_DATA_SIZE 4
