# iris-lm-config

Custom QMK/Vial firmware for the Keebio Iris LM K1.

This is a QMK external userspace repository. The Vial QMK toolchain lives in a separate, upstream-tracking checkout; this repository owns the keymap, Vial export, RGB behavior, validation tools, and project skills.

## Project skills

Pi discovers these skills when this trusted repository is the working directory:

| Skill | Use it for |
|---|---|
| `/skill:firmware-build` | validate and rebuild firmware |
| `/skill:qmk-rgb-sync` | align per-key RGB clusters after Vial edits |
| `/skill:vial-to-kanata` | audit or sync Vial Layer 1 to Kanata |

Typical Vial change:

1. Save the current layout from Vial.
2. Run `/skill:vial-to-kanata`.
3. Run `/skill:qmk-rgb-sync`.
4. Run `/skill:firmware-build`.
5. Flash and test manually.

## Sources of truth

- `keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil` is the canonical saved Vial layout.
- `keymap.c` contains matching firmware defaults and RGB cluster arrays.
- `~/.local/share/chezmoi/dot_config/kanata/config.kbd` is the separate Kanata source.
- `.agents/skills/` contains the project workflows.

`~/.config/vial-qmk/keebio-iris-lm.vil` should be a symlink to the canonical export so Vial saves directly into this repository.

## Manual build

```sh
qmk config user.overlay_dir="$(realpath .)"
qmk userspace-doctor
python3 tools/check_vil_keymap_sync.py
python3 tools/check_rgb_cluster_sync.py
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
```

The generated `.bin` is ignored by Git. Flashing is always a separate manual step.

## Documentation

- [`docs/workflow.md`](docs/workflow.md): manual build, flash, release, and upstream workflow
- [`docs/rgb-clusters.md`](docs/rgb-clusters.md): RGB mapping reference
- [`docs/kanata-sync.md`](docs/kanata-sync.md): manual Vial-to-Kanata mapping
- [`docs/homerow-mods.md`](docs/homerow-mods.md): homerow-mod decision and tuning rationale
