# iris lm — vial_custom keymap

split ergonomic keymap with cluster-based RGB layer indication.

build / flash / vial workflow lives at the repo root: see [`../../../../../README.md`](../../../../../README.md) and [`../../../../../docs/workflow.md`](../../../../../docs/workflow.md).

## files

| file | role |
|---|---|
| `keymap.c` | layout, custom keycodes, RGB cluster definitions + indicator logic |
| `config.h` | vial UID, layer count, split layer state, seed `TAPPING_TERM` (runtime value comes from vial qmk settings — see [`docs/homerow-mods.md`](../../../../../docs/homerow-mods.md)) |
| `rules.mk` | feature flags (`VIA_ENABLE`, `VIAL_ENABLE`, `VIALRGB_ENABLE`) |
| `vial.json` | keyboard layout descriptor compiled into firmware for vial GUI |
| `keebio-iris-lm.vil` | vial GUI export — keymap-as-data. canonical source of truth (see root README) |

## layers

7 layers defined in `enum custom_layer` (keymap.c):

| layer | activation | purpose |
|---|---|---|
| `_MAIN` | base | qwerty |
| `_FN1` | `LT(1, KC_SPACE)` (momentary) | nav / symbols |
| `_FN2` | `LT(2, KC_SPACE)` (momentary) | numbers / fkeys |
| `_FN3` | `LT(3, KC_ENTER)` (momentary) | numpad / media |
| `_FN4` | `TG(4)` (toggle) | homerow-mods-off |
| `_FN5` | `TD(1)` tap dance hold | mouse keys |
| `_FN6` | reserved | — |

## RGB indication model

per-LED color is decided by **cluster classification**, not per-layer single hue.

- each non-`_MAIN` layer has a `fn{n}_clusters[68]` PROGMEM array in `keymap.c`, mapping LED index → cluster type (numbers, symbols, arrows, modifiers, …)
- `cluster_colors[]` in `keymap.c` defines the HSV color for each cluster type
- `rgb_matrix_indicators_advanced_user` in `keymap.c` paints the active layer's array on top of the base animation
- toggle: the `RGB_LYR` keycode (`QK_KB_0`) remains available but unassigned

to keep the cluster arrays in sync with vial keymap changes, run the **`keyb:qmk-rgb` claude agent**. canonical definition lives in this repo at `.claude/agents/keyb.qmk-rgb.md`; `~/.claude/agents/keyb.qmk-rgb.md` is a symlink to it (so the agent is available globally and is versioned alongside the keymap). it parses `keebio-iris-lm.vil`, classifies each keycode, and updates the `fn*_clusters` arrays via Serena.

## customization quick-reference

| change | where |
|---|---|
| cluster colors | `cluster_colors[]` array in `keymap.c` (HSV triplets) |
| keycode → cluster mapping | classification rules in `~/.claude/agents/keyb.qmk-rgb.md`, then re-run the agent |
| add a new layer | extend `enum custom_layer`, add a `fn{n}_clusters` array, add a case in `get_layer_clusters`, bump `DYNAMIC_KEYMAP_LAYER_COUNT` in `config.h` if needed |
| tapping term / hold flavor / chordal hold / flow tap | **vial → QMK Settings tab** (runtime, EEPROM-backed; saves into `.vil`'s `settings` block on file → save). full rationale in [`docs/homerow-mods.md`](../../../../../docs/homerow-mods.md). `config.h`'s `TAPPING_TERM` is only the seed for first boot. |
| keymap itself | edit in vial GUI; save layout to `keebio-iris-lm.vil`; run the `keyb:vial-to-kanata` agent; deploy/restart kanata with `chezmoi apply ~/.config/kanata/config.kbd` + `sudo launchctl kickstart -k system/xbxd.kanata` if that source changed; run the `keyb:qmk-rgb` agent; run `python3 tools/check_vil_keymap_sync.py` from repo root; `qmk compile -kb keebio/iris_lm/k1 -km vial_custom` |

## troubleshooting

- **RGB indicators only show on one half** — `SPLIT_LAYER_STATE_ENABLE` must be defined in `config.h` (it is).
- **vial GUI doesn't recognize the keyboard** — confirm `VIA_ENABLE`/`VIAL_ENABLE` in `rules.mk`, and that the firmware was flashed (`.bin` from the most recent build).
- **firmware too large** — disable RGB matrix effects via `#define`s in `config.h` (see [QMK RGB matrix docs](https://docs.qmk.fm/features/rgb_matrix#additional-configh-options)).

## links

- [QMK RGB matrix](https://docs.qmk.fm/features/rgb_matrix)
- [vial docs](https://get.vial.today/docs/)
- root workflow: [`docs/workflow.md`](../../../../../docs/workflow.md)
