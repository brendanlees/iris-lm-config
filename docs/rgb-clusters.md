# RGB cluster reference

Manual reference for keeping the per-layer RGB arrays in `keymap.c` aligned with the canonical Vial export. Normally use the `qmk-rgb-sync` project skill.

## Clusters

| Token | Meaning | Color |
|---|---|---|
| `_CN` | inactive | off |
| `_CU` | numbers and keypad | yellow |
| `_CS` | ordinary symbols | orange |
| `_CC` | brackets, braces, slash, pipe | cyan |
| `_CA` | arrows | green |
| `_CM` | media and brightness | red |
| `_CO` | modifiers and modifier shortcuts | purple |
| `_CK` | tab, boot, Num Lock, layer controls | pink |
| `_CV` | page and document navigation | sky blue |
| `_CB` | preserve base animation | unchanged |

Transparent keys use `_CN` on momentary layers and `_CB` on toggle layers. Transparent or disabled thumb keys use `_CB`. FN3 Num Lock uses `_CB` while off because runtime code paints LED 42 red while active.

Unclassified app shortcuts, custom keycodes, mouse actions, plain keys on the HRM-off layer, and intentionally colored actions keep their existing cluster until explicitly changed.

## Vial matrix to LED

The `.vil` layout is indexed as `layout[layer][row][column]`. These lists give LED indices for columns 0 through 5. A dash is an unused thumb position.

| Row | Side | LEDs by Vial column 0..5 |
|---|---|---|
| 0 | left top | `0 2 3 5 6 8` |
| 1 | left qwerty | `14 13 12 11 10 9` |
| 2 | left home | `15 16 17 18 19 20` |
| 3 | left bottom | `28 26 25 23 22 21` |
| 4 | left thumb | `- - 33 32 30 29` |
| 5 | right top | `34 36 37 39 40 42` |
| 6 | right qwerty | `48 47 46 45 44 43` |
| 7 | right home | `49 50 51 52 53 54` |
| 8 | right bottom | `62 60 59 57 56 55` |
| 9 | right thumb | `- - 63 64 66 67` |

Underglow LEDs are `1 4 7 24 27 31 35 38 41 58 61 65` and always use `_CN`.

The checked-in mapping comes from `keyboards/keebio/iris_lm/k1/keyboard.json` in the Vial QMK checkout. `tools/check_rgb_cluster_sync.py` carries the same fixed hardware map so CI does not need that checkout.

## Manual sync

1. Save the layout from Vial.
2. Run `python3 tools/check_vil_keymap_sync.py`.
3. Run `python3 tools/check_rgb_cluster_sync.py`.
4. Correct deterministic drift in the matching `fn*_clusters` array.
5. Review changed Vial positions for unclassified keycodes and choose their colors deliberately.
6. Re-run both checks, inspect the diff, and compile the firmware.
