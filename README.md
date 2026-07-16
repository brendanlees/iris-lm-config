# iris-lm-config

custom qmk/vial firmware for keebio iris lm (k1 pcb).

set up as a **qmk external userspace** repository, so the firmware build toolchain (vial-qmk) lives separately and is upstream-clean — no fork modifications.

for day-to-day workflow, see [`docs/workflow.md`](docs/workflow.md).

## layout

```
keyboards/keebio/iris_lm/keymaps/vial_custom/   keymap source (keymap.c, config.h, rules.mk, vial.json, keebio-iris-lm.vil)
docs/                                           design notes + spec/plan history
tools/gen_led_map.py                            led position helper
tools/check_vil_keymap_sync.py                  drift check for `.vil` vs `keymap.c` defaults
.claude/agents/keyb.vial-to-kanata.md          claude code agent for syncing kanata config from vial keymap (symlinked from ~/.claude/agents/)
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

## validate

these checks do not require the qmk cli:

```sh
python3 -m json.tool qmk.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/vial.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil >/dev/null
python3 -m py_compile tools/*.py
python3 tools/check_vil_keymap_sync.py
```

## vial config

`keymaps/vial_custom/keebio-iris-lm.vil` is the keymap-as-data exported from vial. it pairs with a firmware build that includes `vial.json` (compiled in via `rules.mk`).

this file is the canonical source of truth. `~/.config/vial-qmk/keebio-iris-lm.vil` is a symlink to it, so vial gui's default save location writes through to the repo automatically. `keymap.c` contains firmware defaults generated/synced from the same layout; after saving vial, run the `keyb:vial-to-kanata` agent, then the `keyb:qmk-rgb` agent, then `python3 tools/check_vil_keymap_sync.py` to catch drift before flashing.

current known-good vial qmk settings live in the `.vil` `settings` block:

```json
{"7": 200, "25": 150, "26": 1, "27": 0}
```

- `7` tapping term = `200`
- `25` quick tap term = `150`
- `26` chordal hold = enabled
- `27` flow tap = `0`

see [`docs/homerow-mods.md`](docs/homerow-mods.md) for the full tap-hold rationale and Vial runtime override warning.

## releases

tagged firmware releases (`firmware-vN`). the `.bin` is gitignored, so each release is built from the tagged commit before flashing.
