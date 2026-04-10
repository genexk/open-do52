# Open DO52 Build Guide

## Prerequisites

### Install QMK

```bash
pip3 install qmk
qmk setup  # accepts defaults, clones qmk_firmware to ~/qmk_firmware
```

On macOS, you also need the AVR toolchain:

```bash
brew tap osx-cross/avr
brew install avr-gcc@12
export PATH="/opt/homebrew/opt/avr-gcc@12/bin:$PATH"  # add to .zshrc
```

### Link Keyboard

```bash
ln -s /path/to/open-do52/keyboards/open_do52 ~/qmk_firmware/keyboards/open_do52
```

## Compile

### Standard Build (VIA enabled)

```bash
qmk compile -kb open_do52 -km via
```

### DPAD Mode Variants

```bash
qmk compile -kb open_do52 -km via                    # 5-way nav switch (default)
qmk compile -kb open_do52 -km via -e DPAD_MODE=1     # single key replacement
qmk compile -kb open_do52 -km via -e DPAD_MODE=2     # rotary encoder replacement
```

## Flash

### First Time (sets left/right identity)

Each half needs to be flashed once with its identity. This writes an EEPROM flag
that persists across future firmware updates.

1. Connect the **left** half via USB
2. Short RESET to GND on the Pro Micro (enters bootloader for ~8 seconds)
3. Run: `qmk flash -kb open_do52 -km via -bl avrdude-split-left`
4. Disconnect left, connect the **right** half
5. Short RESET to GND
6. Run: `qmk flash -kb open_do52 -km via -bl avrdude-split-right`

### Subsequent Updates

After the initial identity flash, just flash normally (the EEPROM marker persists):

```bash
qmk flash -kb open_do52 -km via
```

Flash each half separately — connect one half, enter bootloader, flash, repeat for
the other half.

## VIA Configuration

1. Download [VIA](https://www.caniusevia.com/)
2. Open VIA, go to Settings, enable "Show Design Tab"
3. Go to the Design tab, click "Load Draft Definition"
4. Load `via/open_do52.json` from this repo
5. Your keyboard should now appear in VIA

### TrackPoint Sensitivity

In VIA, navigate to the custom "TrackPoint" menu. Use the sensitivity slider
(1-255) to adjust cursor speed. The setting is saved to EEPROM automatically.

You can also map the `Sens+` / `Sens-` custom keycodes to any key for on-the-fly
adjustment.

## TrackPoint Notes

- The TrackPoint connects to the **right** half via PS/2 (pins B1 clock, B2 data)
- Thanks to EE_HANDS + split pointing, the TrackPoint works regardless of which
  half has USB connected
- Default sensitivity: 128 (adjustable 1-255 via VIA or keycodes)
- Rotation: 270 degrees (configured in ps2_pointing.c)

## Troubleshooting

### Keyboard not detected
- Check TRRS cable connection between halves
- Verify Pro Micro is properly seated
- Try the other half's USB port

### TrackPoint not moving
- Verify PS/2 wires: B1 = clock, B2 = data (check polarity)
- If USB is on the left side, the TrackPoint data is forwarded via serial —
  ensure TRRS cable is good
- Try adjusting sensitivity up (it may be set very low)

### Flash fails
- Double-tap RESET quickly (some Pro Micro clones need a fast double-tap)
- Check that the correct serial port is detected
- On macOS: `ls /dev/cu.usbmodem*` while in bootloader mode

### VIA doesn't recognize keyboard
- Make sure you loaded the draft definition (`via/open_do52.json`)
- VID must match: 0xFEA6 / PID: 0x0A54
- Try unplugging and reconnecting after loading the definition
