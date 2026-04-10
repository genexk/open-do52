# Split serial driver
SERIAL_DRIVER = bitbang

# PS/2 TrackPoint (low-level driver only — custom pointing device handles the protocol)
PS2_ENABLE = yes
PS2_DRIVER = busywait

# Pointing device (for split forwarding)
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
SRC += ps2_pointing.c

# DPAD mode — pass to C preprocessor and configure encoder if mode 2
ifneq ($(strip $(DPAD_MODE)),)
    OPT_DEFS += -DDPAD_MODE=$(strip $(DPAD_MODE))
endif

ifeq ($(strip $(DPAD_MODE)),2)
    ENCODER_ENABLE = yes
    ENCODER_MAP_ENABLE = yes
else
    ENCODER_ENABLE = no
    ENCODER_MAP_ENABLE = no
endif
