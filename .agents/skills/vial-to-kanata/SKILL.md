---
name: vial-to-kanata
description: Audits and synchronizes selected Iris Vial Layer 1 behavior to the laptop Kanata configuration. Use after saving Vial keymap changes or when the laptop symbol, navigation, homerow-mod, or chord layout has drifted.
compatibility: Requires the local chezmoi source repository and kanata CLI.
---

# Vial to Kanata

Treat the saved Vial export as canonical. Synchronize QMK Layer 1, homerow modifier assignments, and chords to Kanata while preserving device-specific timing.

## Sources

- Vial export: `/Users/brendan/Code/iris-lm-config/keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil`
- Kanata source: `/Users/brendan/.local/share/chezmoi/dot_config/kanata/config.kbd`
- Deployed config: `/Users/brendan/.config/kanata/config.kbd`
- Manual mapping: `../../../docs/kanata-sync.md` relative to this skill directory

## Scope

Synchronize:

- Layer 1 symbols, brackets, arrows, navigation, math, and shortcut keys
- Base-layer homerow modifier assignments, but not timing values
- Active Vial combos and their exact outputs, including keypad variants
- `defsrc`, aliases, chords, and affected layer entries when required

Do not change QMK/Vial files, Kanata device filters, timing values, unrelated layers, or service state.

## Workflow

1. For a fresh Vial edit, confirm the canonical `.vil` was saved. For an explicit audit, no tracked diff is required.
2. Parse `.vil` as JSON. Confirm `layout[0][7][5]` is `KC_H` or its current base-layer variant. Stop if this orientation check fails.
3. Read `defsrc`, `defalias`, `defchordsv2`, and each `deflayer` from the Kanata source.
4. Compare current behavior using `docs/kanata-sync.md`. Preserve keypad versus main-cluster keys and left versus right modifiers where Kanata supports them.
5. Report each category as in sync or drifted. Show the complete proposed Kanata diff and ask for approval before writing.
6. If changes are approved, first create or switch to an isolated feature branch or worktree in the chezmoi repository. Never edit its `main` or `master` checkout.
7. Apply the smallest config change. Update the Kanata README only when user-facing behavior changed.
8. Validate:

```sh
kanata --check -c /Users/brendan/.local/share/chezmoi/dot_config/kanata/config.kbd
```

9. Do not deploy or restart automatically. If requested after validation, apply the source with chezmoi, confirm deployed and source files match, and ask the user to perform any privileged restart interactively.

## Report

Report status for Layer 1, homerow mods, and chords; list any changed aliases or source keys; include validation and deployed/source comparison results; identify any remaining deploy or restart step.
