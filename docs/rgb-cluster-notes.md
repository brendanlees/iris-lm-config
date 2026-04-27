# Iris LM RGB Cluster — Architecture Notes

## Why Static Tables?

Two bugs caused incorrect colors after many iteration attempts:

### Bug 1 — Keycode Ambiguity
`LSFT(KC_9)` = `(` appears at two positions on _FN1 with *different intended colors*:
- LED 17 `[2,2]` home-row: should be `CODE_SYMBOLS` (cyan — bracket in code context)
- LED 45 `[6,3]` right shifted-numbers row: should be `SYMBOLS` (orange — with ^ & * ( ) row)

Keycode-only matching cannot distinguish these. Adding position overrides to fix this introduced Bug 2.

### Bug 2 — Position Overrides Using Wrong Layer Context
The overrides named positions by their MAIN layer key ("/ key at row 8, col 4") but fired
during _FN1 where that matrix position holds a different keycode. Concrete examples:
- `row==8, col==4` → `CLUSTER_MEDIA` (red) fired on `KC_EQUAL` — should be SYMBOLS (orange)
- `row==8, col==5` → `CLUSTER_SPECIAL` (pink) fired on `KC_PGDN` — should be NAVIGATION (sky)

### Solution
Static `PROGMEM uint8_t[68]` arrays per layer. Each entry is the cluster for that LED index.
Completely explicit — no runtime keycode matching. Correct by construction.

---

## LED Index → Matrix Position Reference

Derived from `k1/keyboard.json` `rgb_matrix.layout` (array index = LED index).
Underglow LEDs have `"flags": 2` and no `"matrix"` field.

### Left Half (LED 0–33)
| LED | Matrix | MAIN key      | Notes         |
|-----|--------|---------------|---------------|
| 0   | [0,0]  | ESC           |               |
| 1   | --     | --            | underglow     |
| 2   | [0,1]  | 1             |               |
| 3   | [0,2]  | 2             |               |
| 4   | --     | --            | underglow     |
| 5   | [0,3]  | 3             |               |
| 6   | [0,4]  | 4             |               |
| 7   | --     | --            | underglow     |
| 8   | [0,5]  | 5             |               |
| 9   | [1,5]  | T             | row1 R→L      |
| 10  | [1,4]  | R             |               |
| 11  | [1,3]  | E             |               |
| 12  | [1,2]  | W             |               |
| 13  | [1,1]  | Q             |               |
| 14  | [1,0]  | Tab/TD        |               |
| 15  | [2,0]  | LCtrl/ESC     | home row L→R  |
| 16  | [2,1]  | A (LGUI_T)    |               |
| 17  | [2,2]  | S (LALT_T)    |               |
| 18  | [2,3]  | D (LSFT_T)    |               |
| 19  | [2,4]  | F (LCTL_T)    |               |
| 20  | [2,5]  | G             |               |
| 21  | [3,5]  | B             | bottom R→L    |
| 22  | [3,4]  | V             |               |
| 23  | [3,3]  | C             |               |
| 24  | --     | --            | underglow     |
| 25  | [3,2]  | X             |               |
| 26  | [3,1]  | Z             |               |
| 27  | --     | --            | underglow     |
| 28  | [3,0]  | LShift/OSM    |               |
| 29  | [4,5]  | thumb inner   |               |
| 30  | [4,4]  | thumb mid-out |               |
| 31  | --     | --            | underglow     |
| 32  | [4,3]  | thumb mid     |               |
| 33  | [4,2]  | thumb outer   |               |

### Right Half (LED 34–67)

> **⚠️ Physical chain differs from `keyboard.json` for QWERTY, HOME, and BOTTOM rows.**
> The table below shows the **actual physical key** each LED lights — verified by observation.
> See "Physical LED Chain Discrepancy" section below for full details.

| LED | Matrix | MAIN key         | Notes                       |
|-----|--------|------------------|-----------------------------|
| 34  | [5,0]  | rightmost top    | chain: outer→inner ✓        |
| 35  | --     | --               | underglow                   |
| 36  | [5,1]  | top col1         |                             |
| 37  | [5,2]  | top col2         |                             |
| 38  | --     | --               | underglow                   |
| 39  | [5,3]  | top col3         |                             |
| 40  | [5,4]  | top col4         |                             |
| 41  | --     | --               | underglow                   |
| 42  | [5,5]  | top innermost    |                             |
| 43  | [6,0]  | Y (outermost)    | chain: outer→inner (⚠️ reversed in keyboard.json) |
| 44  | [6,1]  | U                |                             |
| 45  | [6,2]  | I                |                             |
| 46  | [6,3]  | O                |                             |
| 47  | [6,4]  | P                |                             |
| 48  | [6,5]  | BSPC (innermost) |                             |
| 49  | [7,5]  | ' (innermost)    | chain: inner→outer (⚠️ reversed in keyboard.json) |
| 50  | [7,4]  | ;                |                             |
| 51  | [7,3]  | L                |                             |
| 52  | [7,2]  | K                |                             |
| 53  | [7,1]  | J                |                             |
| 54  | [7,0]  | H (outermost)    |                             |
| 55  | [8,0]  | N (outermost)    | chain: outer→inner (⚠️ reversed in keyboard.json) |
| 56  | [8,1]  | M                |                             |
| 57  | [8,2]  | ,                |                             |
| 58  | --     | --               | underglow                   |
| 59  | [8,3]  | .                |                             |
| 60  | [8,4]  | /                |                             |
| 61  | --     | --               | underglow                   |
| 62  | [8,5]  | RShift/OSM (innermost) |                       |
| 63  | [9,2]  | thumb outer      |                             |
| 64  | [9,3]  | thumb mid        |                             |
| 65  | --     | --               | underglow                   |
| 66  | [9,4]  | thumb mid-out    |                             |
| 67  | [9,5]  | thumb inner      |                             |

---

## Key Facts About the Matrix

- Right half matrix columns: col 0 = outermost (right edge), col 5 = innermost (near split).
  `[7,0]` = H (outermost home), `[7,5]` = `'` (innermost). This is correct at the matrix level.
- The LAYOUT macro lists right-side keys inner→outer (slot 0 = innermost).
  Vial's visual layout uses the same ordering: Vial col 0 = innermost, col 5 = outermost.
- Underglow LED indices: 1, 4, 7, 24, 27, 31, 35, 38, 41, 58, 61, 65 (12 total).
  Always `CLUSTER_NONE` in every layer array.
- Thumb cluster LEDs: 29–33 (left) and 63–67 (right).
  On FN3, LEDs 64 and 66 hold active numpad keys (KP_0 and KP_DOT).

---

## Physical LED Chain Discrepancy (Right Half)

`keyboard.json`'s `rgb_matrix.layout` documents the right-half LED chain direction
**incorrectly** for the QWERTY, HOME, and BOTTOM rows. The TOP row is correct.

| Row    | keyboard.json says | Physical reality | LEDs  |
|--------|--------------------|------------------|-------|
| TOP    | outer → inner      | outer → inner ✓  | 34–42 |
| QWERTY | inner → outer      | outer → inner ⚠️  | 43–48 |
| HOME   | outer → inner      | inner → outer ⚠️  | 49–54 |
| BOTTOM | inner → outer      | outer → inner ⚠️  | 55–62 |

**Consequence:** When assigning clusters in `fn*_clusters[]`, use the LED index → MAIN key
mapping from the **Right Half table above** (physical reality), not from `keyboard.json`.

**How this was discovered:** After flashing the static PROGMEM table implementation,
6 right-side keys on FN1 showed wrong colours. Binary verification confirmed the firmware
was correct per `keyboard.json`. The symptoms were consistent only with the physical chain
running in the opposite direction for QWERTY, HOME, and BOTTOM rows. After correcting the
`fn1_clusters` assignments to match the physical chain, all 6 issues were resolved.

**The matrix positions themselves are correct** — `keyboard.json`'s `layouts.LAYOUT` and the
Vial `.vil` keycode arrays accurately reflect which keycode is at which matrix position.
Only the `rgb_matrix.layout` LED chain ordering is wrong for those three rows.

---

## Cluster Colors (Tokyo Night palette)

| Cluster              | Shorthand | Color     | HSV Hue | Notes                                   |
|----------------------|-----------|-----------|---------|----------------------------------------|
| CLUSTER_NUMBERS      | `_CU`     | Yellow    | 28      |                                         |
| CLUSTER_SYMBOLS      | `_CS`     | Orange    | 12      |                                         |
| CLUSTER_CODE_SYMBOLS | `_CC`     | Cyan      | 154     |                                         |
| CLUSTER_ARROWS       | `_CA`     | Green     | 72      |                                         |
| CLUSTER_MEDIA        | `_CM`     | Red       | 242     |                                         |
| CLUSTER_MODIFIERS    | `_CO`     | Purple    | 188     |                                         |
| CLUSTER_SPECIAL      | `_CK`     | Pink      | 213     |                                         |
| CLUSTER_NAVIGATION   | `_CV`     | Sky blue  | 141     |                                         |
| CLUSTER_BASE         | `_CB`     | (none)    | —       | Skip indicator — base animation shows through |
| CLUSTER_NONE         | `_CN`     | Black     | —       | Force LED off (0, 0, 0)                |

### CLUSTER_BASE vs CLUSTER_NONE

- `_CN` (CLUSTER_NONE) forces the LED to black — the key is visually off
- `_CB` (CLUSTER_BASE) skips `rgb_matrix_set_color` entirely — the base animation (white) shows through

Use `_CB` for thumb keys so they always show the ambient backlight regardless of active layer.
Use `_CN` for inactive keys that should be dark (TRNS keys, removed keycodes).

---

## Adding Future Layers

When adding keymaps to FN4–FN6:
1. Add a `fn4_clusters[68] PROGMEM` array in keymap.c following the same format
2. Add `case _FN4: return fn4_clusters;` to `get_layer_clusters()`
3. No other code changes needed

**Thumb keys:** Always use `_CB` for the 8 thumb LEDs (29, 30, 32, 33, 63, 64, 66, 67) so they show the base animation. Only use `_CU`/`_CS`/etc. on a thumb LED if it has a meaningful active keycode you want to call out.

**When assigning right-side cluster values:**
- Use the **Right Half table above** for the LED → physical key mapping
- Cross-reference keycodes from `keymap.c` (uses LAYOUT inner→outer slot order)
- **Do not use `keyboard.json` rgb_matrix.layout** for QWERTY/HOME/BOTTOM row LED positions —
  those rows are documented with the wrong chain direction (see Physical LED Chain Discrepancy)

**⚠️ vial.vil row ordering is REVERSED for right-side rows:**
`vial.vil` stores right-side rows **outer → inner** (rightmost key first, pos 0), which is
**opposite** to the LAYOUT slot order (inner → outer, slot 0 = innermost). Do not use
`vial.vil` alone to infer LED positions — cross-reference with `gen_led_map.py` output or
`keymap.c`. Example for top row: `vial.vil` pos 0 = BSPC (rightmost, LED 34), pos 5 = KC_6
(innermost/leftmost, LED 42).

---

## Known Bugs / Future Work

### fn3_clusters QWERTY Row — Chain Reversal (Unfixed)

The fn3 right-side QWERTY row (LEDs 43–48) has the same chain-reversal bug that fn1 and fn2 had:
- LED 43 (physical = Y = KC_KP_7) is set to `_CN` → Y shows black
- LED 48 (physical = BSPC = KC_TRNS) is set to `_CU` → BSPC spuriously shows yellow

This is visible when holding FN3 (app switcher + numpad layer). Fix when working on FN3 colours using the same correction applied to fn1 and fn2.
