# Homerow mods decision

The base layer uses mirrored homerow modifiers:

| Key | Tap | Hold |
|---|---|---|
| `a s d f` | letters | left Cmd, Alt, Shift, Ctrl |
| `j k l ;` | letters | right Ctrl, Shift, Alt, left Cmd |

The goal is immediate cross-hand shortcuts without turning fast same-hand rolls into modifiers. This follows the positional idea from [urob's timeless homerow mods](https://github.com/urob/zmk-config#timeless-homerow-mods).

## Current Vial runtime settings

| Setting | Value | Reason |
|---|---:|---|
| Chordal Hold | on | opposite-hand presses commit a hold immediately |
| Flow Tap | `0` | avoids forcing a pending mod-tap to tap during fast typing |
| Permissive Hold | off | protects same-hand rolls |
| Hold On Other Key Press | off | avoids promoting every roll to a hold |
| Tapping Term | `150 ms` | same-hand and standalone timeout |
| Quick Tap Term | `150 ms` | repeat behavior for rapid double taps |

Expected behavior:

- Right `j` plus left `b` produces `Ctrl+B` immediately.
- Right `;` plus left `c` produces `Cmd+C` immediately.
- Fast `asdf`, `jkl;`, `kalil`, and `sales` remain letters.
- A deliberate same-hand modifier requires holding past the tapping term.
- FN4 disables homerow mods by replacing those positions with plain keys.

## Why `config.h` is not authoritative

Vial QMK reads tap-hold behavior from its EEPROM-backed QMK Settings values. The `.vil` `settings` block records those runtime values only after **File > Save Current Layout**.

`config.h` keeps `TAPPING_TERM 200` only as the first-boot or EEPROM-reset seed. After Vial writes settings, the saved runtime value of `150 ms` is authoritative. The same runtime override applies to Chordal Hold and Flow Tap.

If behavior changes unexpectedly, inspect Vial's QMK Settings and save the layout before editing firmware defines.

## Smoke test

1. Fast-type `the quick brown fox jumps over the lazy dog`, `asdf jkl;`, `kalil`, and `sales`.
2. Repeat `Ctrl+B`, `Cmd+C`, `Cmd+V`, and `Cmd+T` from opposite hands.
3. Hold `a` past 150 ms, then press `s`; it should produce `Cmd+S`.
4. Toggle FN4 and confirm the homerow becomes plain letters, then toggle it off.

## References

- [QMK Chordal Hold](https://docs.qmk.fm/tap_hold#chordal-hold)
- [QMK Flow Tap](https://docs.qmk.fm/tap_hold#flow-tap)
- [urob's ZMK configuration](https://github.com/urob/zmk-config#timeless-homerow-mods)
