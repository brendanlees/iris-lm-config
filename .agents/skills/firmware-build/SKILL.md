---
name: firmware-build
description: Validates and rebuilds the Iris LM Vial firmware in this repository. Use after keymap, RGB, Vial, config, or rules changes, or when a fresh firmware binary is needed.
compatibility: Requires qmk-cli configured with vial-qmk as QMK_HOME and this repository as QMK_USERSPACE.
---

# Firmware build

Build the `keebio/iris_lm/k1` keyboard with the `vial_custom` keymap. Do not flash, commit, push, tag, or release unless the user separately requests it.

## Preflight

From the repository root:

1. Run `qmk env` and confirm:
   - `QMK_HOME` points to the Vial QMK checkout.
   - `QMK_USERSPACE` points to this repository.
2. Run `qmk userspace-doctor`. Stop and explain any userspace configuration failure.
3. Confirm the canonical Vial export exists at `keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil`.

Do not pull or modify the Vial QMK checkout automatically.

## Validate

Run these before compiling:

```sh
python3 -m json.tool qmk.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/vial.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil >/dev/null
python3 -m py_compile tools/*.py
python3 tools/check_vil_keymap_sync.py
python3 tools/check_rgb_cluster_sync.py
```

Stop if validation fails. Do not produce a new binary from known-drifted sources.

## Build

```sh
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
```

Report the compile result and the generated `.bin` path. State explicitly that the firmware was built but not flashed.

For manual setup, flashing, releases, or upstream updates, see `../../../docs/workflow.md` relative to this skill directory.
