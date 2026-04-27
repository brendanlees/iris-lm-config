# Iris LM - Vial Keymap

Split ergonomic keyboard with custom RGB layer indication.

## Features
- RGB highlights configured keys per layer
- Layer colors: FN1=green, FN2=cyan, FN3=red
- Toggle: `RGB_LYR` keycode
- 100ms activation delay (prevents flash on quick taps)

## Setup
```bash
# One-time config
qmk config user.overlay_dir="$HOME/.config/keyboards/vial-qmk"

# Install toolchain (if needed)
brew install arm-none-eabi-gcc@8 arm-none-eabi-binutils
```

## Build & Flash
```bash
make compile  # Or: qmk compile -kb keebio/iris_lm/k1 -km vial
make flash    # Or: qmk flash -kb keebio/iris_lm/k1 -km vial
```

**Bootloader mode:** Press reset button OR hold both top corner keys while plugging in.

## Customization

### Change Layer Colors
Edit `rgb_layers.c`:
```c
switch (layer) {
    case _FN1:
        hue = 85;  // Change this (0-255)
        break;
    // ...
}
```

**Hue chart:** 0=red, 43=orange, 85=green, 128=cyan, 170=blue, 213=purple

### Add New Layer
1. Add to enum in `keymap.c`:
```c
enum custom_layer {
    _MAIN, _FN1, _FN2, _FN3,
    _FN4,  // NEW
};
```

2. Add layer definition in `keymap.c`

3. Add color in `rgb_layers.c`:
```c
case _FN4:
    hue = 170;  // Blue
    break;
```

### Adjust Activation Delay
Edit `config.h`:
```c
#define LAYER_LED_ACTIVATION_TIME 100  // Change milliseconds
```

### Add RGB_LYR Toggle
In Vial GUI: Any keycode → type `RGB_LYR`

## Make Guide

| Command | Action |
|---------|--------|
| `make` | Show help |
| `make compile` | Build firmware |
| `make flash` | Build + flash |
| `make clean` | Remove build files |

Run from: `~/.config/keyboards/vial-qmk/`

## Troubleshooting

**"arm-none-eabi-gcc not found"**
```bash
export PATH="/opt/homebrew/opt/arm-none-eabi-gcc@8/bin:/opt/homebrew/opt/arm-none-eabi-binutils/bin:$PATH"
```

**"No such file or directory"**  
Run from userspace root: `cd ~/.config/keyboards/vial-qmk`

**"Firmware too large"**  
Uncomment effect disables in `config.h`

**LEDs don't change**  
Hold layer >100ms. Check `LAYER_LED_ACTIVATION_TIME` in `config.h`.

**Vial doesn't recognize**  
Verify `vial.json` exists. Flash with `make flash`.

## Files
```
config.h      - Configuration (RGB delay, effect disables)
keymap.c      - Keymap + RGB_LYR keycode
rgb_layers.c  - RGB layer indication logic
rgb_layers.h  - RGB header
rules.mk      - Build rules
vial.json     - Vial layout
```

## Links
- [QMK RGB Matrix](https://docs.qmk.fm/features/rgb_matrix)
- [Vial Docs](https://get.vial.today/docs/)
