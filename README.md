# iris-lm-firmware

Custom QMK/Vial firmware for the [Keebio Iris LM (k1 PCB)](https://keeb.io/products/iris-lm-keyboard) — `vial_custom` keymap with per-layer cluster RGB indicators, Tokyo Night palette, FN3 numpad layer, and tap-dance / homerow-mod ergonomics.

This is a **QMK External Userspace** repository. The firmware build toolchain (vial-qmk) lives separately at `~/Code/vial-qmk/` and is upstream-clean (no fork modifications).

## Layout

```
keyboards/keebio/iris_lm/keymaps/vial_custom/   keymap source (keymap.c, config.h, rules.mk, vial.json, vial.vil)
docs/                                           design notes + spec/plan history
tools/gen_led_map.py                            LED position helper
qmk.json                                        userspace build target manifest
```

## Build

One-time setup (already done on this machine):

```sh
qmk config user.overlay_dir="$(realpath ~/Code/iris-lm-firmware)"
qmk userspace-doctor   # should report "Userspace enabled: True"
```

Compile:

```sh
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
# OR build all targets in qmk.json:
qmk userspace-compile
```

The `.bin` file lands in this repo's root. Flash with QMK Toolbox or `qmk flash`.

## Vial config

`keymaps/vial_custom/vial.vil` is the keymap-as-data exported from Vial. It pairs with a firmware build that includes `vial.json` (compiled in via `rules.mk`).

## Releases

Tag firmware releases on this repo (`firmware-vN`). The `.bin` is gitignored, so each release should be built from the tagged commit before flashing.
