# Open DO52

Open-source QMK firmware for the YK DO52 split keyboard with ThinkPad TrackPoint.

## Features

- Full QMK support with VIA configurator
- EE_HANDS — plug USB into either half
- Adjustable TrackPoint sensitivity (VIA slider + keycodes)
- DPAD mode selection: 5-way nav switch, single key, or rotary encoder

## Quick Start

1. Set up [QMK](https://docs.qmk.fm/#/newbs_getting_started)
2. Symlink this keyboard into your QMK tree:
   ```
   ln -s $(pwd)/keyboards/open_do52 ~/qmk_firmware/keyboards/open_do52
   ```
3. Compile: `qmk compile -kb open_do52 -km via`
4. Flash each half once:
   ```
   qmk flash -kb open_do52 -km via -bl avrdude-split-left   # left half
   qmk flash -kb open_do52 -km via -bl avrdude-split-right  # right half
   ```

## DPAD Modes

Build with a different DPAD mode:

```
qmk compile -kb open_do52 -km via -e DPAD_MODE=0   # 5-way nav (default)
qmk compile -kb open_do52 -km via -e DPAD_MODE=1   # single key
qmk compile -kb open_do52 -km via -e DPAD_MODE=2   # rotary encoder
```

## Pin Map

| Pin | Function |
|-----|----------|
| D3, D2, D1, D4, C6, D7 | Matrix columns 0-5 |
| B4, B5, F4, E6, F5, F6 | Matrix rows 0-5 |
| D0 | Split serial |
| B1 | PS/2 TrackPoint clock |
| B2 | PS/2 TrackPoint data |

## Credits

Pin mapping reverse-engineered from YK manufacturer firmware. Reference repos:
- [Canorus/do42-do52](https://github.com/Canorus/do42-do52) — DO42 QMK source
- [ilfmoussa/do52pro](https://github.com/ilfmoussa/do52pro) — DO52 RP2040 port
- [RebezovAndrei/do52-keyboard](https://github.com/RebezovAndrei/do52-keyboard) — Original firmware + VIA JSON

## License

GPL-2.0 (matches QMK)
