# iris-lm-config

custom qmk/vial firmware for keebio iris lm (k1 pcb).

set up as a **qmk external userspace** repository, so the firmware build toolchain (vial-qmk) lives separately and is upstream-clean — no fork modifications.

for day-to-day workflow, see [`docs/workflow.md`](docs/workflow.md).

## layout

```
keyboards/keebio/iris_lm/keymaps/vial_custom/   keymap source (keymap.c, config.h, rules.mk, vial.json, keebio-iris-lm.vil)
docs/                                           design notes + spec/plan history
tools/gen_led_map.py                            led position helper
.claude/agents/keyb.qmk-rgb.md                  claude code agent for syncing rgb cluster arrays with vial keymap (symlinked from ~/.claude/agents/)
qmk.json                                        userspace build target manifest
```

## build

```sh
qmk config user.overlay_dir="$(realpath .)"
qmk userspace-doctor   # should report "userspace enabled: true"
```

## compile:

```sh
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
# or build all targets in qmk.json:
qmk userspace-compile
```

the `.bin` file lands in this repo's root. flash with `qmk flash`.

## vial config

`keymaps/vial_custom/keebio-iris-lm.vil` is the keymap-as-data exported from vial. it pairs with a firmware build that includes `vial.json` (compiled in via `rules.mk`).

this file is the canonical source of truth. `~/.config/vial-qmk/keebio-iris-lm.vil` is a symlink to it, so vial gui's default save location writes through to the repo automatically.

## releases

tagged firmware releases (`firmware-vN`). the `.bin` is gitignored, so each release is built from the tagged commit before flashing.
