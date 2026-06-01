# homerow mods — timeless tuning

inspired by [urob's timeless homerow mods](https://github.com/urob/zmk-config#timeless-homerow-mods) (zmk; ported to qmk and kanata in the wild). the goal: cross-hand mod combos commit instantly via **positional priority**, same-hand rolls keep tapping cleanly via the timer. no more `Ctrl+B` (tmux) flubbing to `Ctrl+Space`, no more flaky `Cmd+C` mid-typing.

## the setup

| key | tap | hold |
|---|---|---|
| `a` | a | Cmd (LGUI) |
| `s` | s | Alt (LALT) |
| `d` | d | Shift (LSFT) |
| `f` | f | Ctrl (LCTL) |
| `j` | j | Ctrl (RCTL) |
| `k` | k | Shift (RSFT) |
| `l` | l | Alt (LALT) |
| `;` | ; | Cmd (LGUI) |

defined in `keymap.c` (`_MAIN` layer) via `LGUI_T(KC_A)`, `LCTL_T(KC_F)`, etc. tap-vs-hold disambiguation happens in firmware via the runtime knobs below.

## the knobs that matter (vial → "QMK Settings" tab)

| setting | value | role |
|---|---|---|
| **Chordal Hold** | ☑ on | opposite-hand keypress commits the pending mod-tap to hold immediately. **the core feature** — without this, every cross-hand mod combo is at the mercy of the tapping term. |
| **Flow Tap** | `0` | disables the recent-typing → force-tap window. with flow tap > 0, any mod-tap pressed within N ms of a letter is forced to act as a tap — which bypasses chordal-hold and produces the exact flubs this whole setup is meant to fix. |
| **Permissive Hold** | ☐ off | with chordal-hold on, cross-hand commits happen on press. permissive-hold only affects **same-hand**, where it can flip "as" (release-S-before-A) into `Cmd+S`. off protects same-hand rolls. |
| **Tapping Term** | `200` | how long to hold a mod-tap before it commits to hold on **same-hand** input. cross-hand is instant via chordal-hold and doesn't consult this. |
| **Quick Tap Term** | `150` | unrelated to mods; controls double-tap-and-hold-becomes-repeat behavior. |
| **Hold On Other Key Press** | ☐ off | would force commit on **any** other keypress including same-hand, breaking rolls. always off. |
| **Retro Tapping** | ☐ off | unrelated; keep off. |

## behaviour predictions

| scenario | result |
|---|---|
| right-J ctrl + left-B (tmux `C-b`) | instant `Ctrl+B` — chordal-hold commits on opposite-hand press |
| right-`;` cmd + left-C (`Cmd+C`) | instant `Cmd+C`, same mechanism |
| fast same-hand roll: "as", "kl", "asdf" | letters; permissive-hold off means release timing doesn't flip them to mods |
| deliberate same-hand mod: hold A past 200 ms, press S | `Cmd+S` — falls back to tapping term once chordal-hold's opposite-hand fast path doesn't apply |
| `_FN4` "HRMs off" toggle layer | unchanged — that layer maps the homerow back to plain `KC_A` / `KC_S` / etc., so chordal-hold has nothing to act on |
| bottom-row tap-dance macros (z/x/c/v/b → Cmd+letter) | unaffected — chordal-hold only touches mod-tap, not tap-dance |

## the pitfall (read this before editing `config.h`)

`vial-qmk` ships **strong** implementations of `get_tapping_term`, `get_permissive_hold`, `get_chordal_hold`, `get_flow_tap_term` in `quantum/qmk_settings.c`. they read from an EEPROM-backed `QS` struct that vial gui writes to. **runtime values always win.**

what this means in practice:
- `#define TAPPING_TERM 200` in `config.h` is ignored at runtime. the value Vial wrote to EEPROM is what the firmware uses.
- `#define CHORDAL_HOLD` is added globally by `builddefs/build_vial.mk:32` for every vial build, so the feature compiles in — but `get_chordal_hold()` returns `false` unless the **Chordal Hold** checkbox is ticked in Vial. compile-time alone does nothing.
- same for `FLOW_TAP_TERM=321` (also from build_vial.mk) — vial overrides it to whatever's in EEPROM (currently `0`).

so if cross-hand combos start flubbing again, **don't go editing `config.h` looking for the answer**. open Vial → QMK Settings tab → check the values above.

`config.h`'s `TAPPING_TERM 200` exists only as the **seed** value Vial uses on first boot / eeprom reset. once Vial has written to EEPROM, that's the truth.

## where the settings are stored

each Vial GUI change persists to two places:
1. **the keyboard's EEPROM** — immediate effect, lives across reboots
2. **`keebio-iris-lm.vil`'s `settings` block** — only when you do file → save in Vial. this is what version control sees.

the indices map (from `vial-qmk/quantum/qmk_settings.c`):

| index | field | mapping |
|---|---|---|
| 7 | tapping_term | Tapping Term |
| 22 | tapping_v2 bit 0 | Permissive Hold |
| 25 | quick_tap_term | Quick Tap Term |
| 26 | tapping_v2 bit 3 | Chordal Hold |
| 27 | flow_tap_term | Flow Tap |

after tweaking in Vial, **save the layout** so the `.vil` reflects the change. otherwise the keyboard and the repo drift silently (see workflow.md phase 4 note on drift).

## verification (smoke tests)

run in this order — highest-risk regressions first, then the wins:

1. **same-hand rolls don't regress**
   - fast-type `the quick brown fox jumps over the lazy dog`, then `asdf jkl;`, then `kalil`, then `sales`
   - every character must be a letter; no stray Cmd/Ctrl/Alt/Shift firing
2. **cross-hand fixes land**
   - in tmux: `Ctrl+B` (right-J + left-B) ten times in a row mid-command — prefix every time
   - in any app: `Cmd+C` (right-`;` + left-C), `Cmd+V`, `Cmd+T`
3. **same-hand deliberate mod still possible**
   - hold left-A for a beat past the tapping term, then press S → `Cmd+S` save dialog
4. **`_FN4` escape hatch intact**
   - toggle FN4 on, homerow becomes plain letters, toggle back off

if step 1 regresses: chordal-hold isn't engaging correctly, or permissive-hold got turned back on. re-check the table above.
if step 2 still flubs after the changes: flow tap isn't at 0 (most likely cause), or tapping term is too short and chordal-hold isn't engaging because of some other interaction — bump tapping term to 240 and retry.

## revert

in Vial → QMK Settings:
- Chordal Hold: ☐ off
- Flow Tap: `200` (or whatever vial-qmk's default was)
- Permissive Hold: ☑ on
- Tapping Term: `150`

then file → save layout to push the change into the `.vil`.

## references

- urob's original (ZMK): <https://github.com/urob/zmk-config#timeless-homerow-mods>
- QMK port writeup (reddit): <https://www.reddit.com/r/ErgoMechKeyboards/comments/1q1jo3c/urobs_zmk_timeless_home_row_mods_ported_to_native/>
- kanata port writeup (reddit): <https://www.reddit.com/r/ErgoMechKeyboards/comments/1qosvc0/a_try_on_urobs_timeless_home_row_mods_for_kanata/>
- QMK chordal-hold docs: <https://docs.qmk.fm/tap_hold#chordal-hold>
- QMK flow-tap docs: <https://docs.qmk.fm/tap_hold#flow-tap>
