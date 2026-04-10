# Open DO52

Open-source QMK firmware for the YK DO52 split keyboard.

- Keyboard Maintainer: [genexk](https://github.com/genexk)
- Hardware Supported: Pro Micro ATmega32U4 (or clone)
- Hardware Availability: YK DO52 PCB

## Features

- EE_HANDS: plug USB into either side
- ThinkPad TrackPoint support (PS/2, right half)
- VIA-adjustable TrackPoint sensitivity
- Configurable DPAD mode (5-way nav / single key / rotary encoder)

## Build

```
qmk compile -kb open_do52 -km via
```

## Flash

One-time per half (sets left/right identity in EEPROM):

```
qmk flash -kb open_do52 -km via -bl avrdude-split-left
qmk flash -kb open_do52 -km via -bl avrdude-split-right
```

## Bootloader

Short RESET to GND on the Pro Micro to enter bootloader.
