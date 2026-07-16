# Vial Sync Workflow Repair Design

## Goal

Restore a reliable workflow from the canonical Vial export to firmware defaults, RGB cluster indicators, and the laptop Kanata configuration while keeping the existing specialist-agent model.

## Decisions

- The saved Vial export remains canonical.
- FN2 top-left becomes transparent; the old `RGB_LYR` compiled default is removed.
- Apply deterministic RGB classifications only. Preserve current colors for ambiguous positions.
- Kanata mirrors exact QMK variants where supported: left Meta for the semicolon homerow mod and keypad-minus for the W+E chord.
- Deploy the validated Kanata source through chezmoi and restart its service.
- Do not add a coordinator prompt, flash firmware, push branches, or create releases.

## Components

### Firmware synchronization

Update the static `LAYOUT(...)` default in `keymap.c` to match the canonical `.vil`. Update the 21 deterministic `fn*_clusters` entries identified by the audit. Do not alter the 31 ambiguous entries or unrelated RGB behavior.

### Kanata synchronization

Update the chezmoi source `dot_config/kanata/config.kbd` for exact QMK parity. Validate with `kanata --check`, apply with chezmoi, restart the Kanata service, and verify the deployed config matches the source.

### Agent definitions

Version both keyboard agent definitions in `iris-lm-config`. Keep Claude and Pi discovery entries linked to those canonical definitions. Change the Kanata agent from the missing `dot_config/vial-qmk/...` input to the canonical `.vil` in `iris-lm-config`, and use absolute paths for the chezmoi source to make execution independent of cwd.

The RGB agent remains specialized: it updates cluster arrays and compiles. The Kanata agent remains specialized: it updates selected Layer 1 behavior and validates Kanata.

### Documentation

Update repository workflow documentation to show both Claude and Pi invocation and the ordered process:

1. Save the current layout from Vial.
2. Run Kanata synchronization.
3. Run RGB synchronization.
4. Run `.vil`/`keymap.c` drift validation.
5. Compile firmware.
6. Apply and restart Kanata when its source changed.

## Error Handling

- Stop if Vial has not been saved to disk or the repository remains unexpectedly unchanged.
- Require approval before resolving newly ambiguous RGB classifications.
- Do not deploy Kanata unless syntax validation passes.
- Do not claim firmware readiness unless QMK compilation succeeds.
- Do not flash hardware automatically.

## Verification

- Validate `qmk.json`, `vial.json`, and `.vil` as JSON.
- Run `python3 tools/check_vil_keymap_sync.py`.
- Re-audit RGB arrays against the canonical `.vil`; expect zero deterministic drift and only preserved ambiguities.
- Run `qmk compile -kb keebio/iris_lm/k1 -km vial_custom`.
- Run `kanata --check` against the chezmoi source.
- Apply chezmoi, restart Kanata, and compare deployed/source config content.
- Confirm no unintended tracked changes or staged files in either repository.

## Scope Boundaries

No firmware flashing, commits beyond the implementation branches, pushes, releases, timing-value changes, layer expansion, or unrelated refactoring.
