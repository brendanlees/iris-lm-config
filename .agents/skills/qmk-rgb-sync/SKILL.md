---
name: qmk-rgb-sync
description: Synchronizes Iris LM per-layer RGB cluster arrays with the canonical Vial layout. Use after saving keymap changes from Vial or when RGB indicators appear on the wrong keys.
compatibility: Python 3 is required for drift checks; qmk-cli is required for final firmware compilation.
---

# QMK RGB sync

Keep `fn*_clusters` in `keymap.c` aligned with the canonical Vial export. Never change the keymap itself, flash firmware, or resolve an ambiguous color without user approval.

## Sources

- Vial layout: `keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil`
- RGB arrays: `keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c`
- Deterministic check: `tools/check_rgb_cluster_sync.py`
- Manual reference: `../../../docs/rgb-clusters.md` relative to this skill directory

## Workflow

1. If this follows a fresh Vial edit, confirm the canonical `.vil` was saved. A tracked diff or explicit user confirmation is sufficient. If a change was expected but no disk change exists, stop and ask the user to use Vial's **File > Save Current Layout**.
2. Run `python3 tools/check_vil_keymap_sync.py`. Firmware defaults must match the Vial export before RGB work.
3. Run `python3 tools/check_rgb_cluster_sync.py`.
4. For deterministic drift, use the checker's layer, LED, matrix position, keycode, and expected cluster to prepare a complete proposed diff.
5. Inspect changed Vial positions for unclassified keycodes using `docs/rgb-clusters.md`. Preserve their current cluster until the user chooses one.
6. Show all proposed changes and ambiguous choices before editing.
7. After approval, update only the affected `fn*_clusters` values and their keycode comments. Do not alter RGB indicator logic or cluster colors.
8. Re-run both sync checks and inspect the focused `keymap.c` diff.
9. Compile with `qmk compile -kb keebio/iris_lm/k1 -km vial_custom`.

## Report

Report changed LEDs by layer, any ambiguous decisions, both sync-check results, and the compile result. State that firmware was not flashed.
