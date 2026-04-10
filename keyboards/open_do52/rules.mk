# Split serial driver
SERIAL_DRIVER = bitbang

# PS/2 TrackPoint
PS2_MOUSE_ENABLE = yes
PS2_ENABLE = yes
PS2_DRIVER = busywait

# Pointing device (for split forwarding)
POINTING_DEVICE_ENABLE = yes
POINTING_DEVICE_DRIVER = custom
SRC += ps2_pointing.c

# Keyboard-level source
SRC += open_do52.c

# DPAD mode conditional
ifeq ($(strip $(DPAD_MODE)),2)
    ENCODER_ENABLE = yes
    ENCODER_MAP_ENABLE = yes
endif
