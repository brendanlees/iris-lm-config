# Firmware workflow

Manual runbook for editing, building, flashing, and releasing this firmware. Prefer the project skills in the root README for normal work.

## Repository model

| Repository | Purpose | Commit policy |
|---|---|---|
| `iris-lm-config` | keymap, Vial export, RGB behavior, tools, skills | commit meaningful changes |
| `vial-qmk` | upstream build toolchain | track upstream; do not add userspace changes |

QMK links them through persistent CLI settings:

```sh
qmk config user.qmk_home="$HOME/Code/vial-qmk" user.overlay_dir="$(realpath .)"
qmk userspace-doctor
```

`qmk env` should show this repository as `QMK_USERSPACE`.

## Choose the workflow

### Vial keymap change

1. In Vial, use **File > Save Current Layout**.
2. Run `/skill:vial-to-kanata`.
3. If Kanata changed, validate and deploy it as described in [`kanata-sync.md`](kanata-sync.md).
4. Run `/skill:qmk-rgb-sync`.
5. Run `/skill:firmware-build`.
6. Flash, test on hardware, then commit and push the source changes.

Vial writes live keyboard state immediately but does not update the tracked `.vil` until it is saved.

### Vial QMK Settings change

Settings-tab changes write to EEPROM immediately. Save the layout so the `.vil` settings block records them. No firmware build is required unless source also changed. See [`homerow-mods.md`](homerow-mods.md).

### Source-only change

For `keymap.c`, `config.h`, or `rules.mk` changes, run `/skill:firmware-build`, flash, test, and then commit. Run the RGB or Kanata skill only when their behavior may have changed.

## Manual validation and build

```sh
python3 -m json.tool qmk.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/vial.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil >/dev/null
python3 -m py_compile tools/*.py
python3 tools/check_vil_keymap_sync.py
python3 tools/check_rgb_cluster_sync.py
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
```

Flash with QMK Toolbox or, when bootloader permissions are configured:

```sh
qmk flash -kb keebio/iris_lm/k1 -km vial_custom
```

Always test the physical keyboard before treating a build as known-good.

## Releases

Tag the tested source, rebuild from that commit, and optionally attach the ignored binary:

```sh
git tag firmware-v2 -m "describe the known-good firmware"
git push origin firmware-v2
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
gh release create firmware-v2 keebio_iris_lm_k1_vial_custom.bin --notes "release notes"
```

## Updating Vial QMK

The toolchain checkout should remain an upstream tracker:

```sh
cd "$HOME/Code/vial-qmk"
git pull --ff-only origin vial
```

Return here and rebuild. Do not pull automatically during an ordinary firmware build.

QMK CLI config normally lives at `~/Library/Application Support/qmk/qmk.ini` on macOS and `~/.config/qmk/qmk.ini` on Linux. Use `qmk config` rather than editing it directly.
