# Vial Sync Workflow Repair Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Make the saved Vial export reliably synchronize firmware defaults, RGB cluster indicators, and the deployed Kanata configuration through working Claude/Pi specialist agents.

**Architecture:** Keep `keebio-iris-lm.vil` as the single input. The firmware and Kanata changes remain in their existing repositories, while both harnesses discover versioned agent definitions from `iris-lm-config`. Each repository is changed in its own Worktrunk worktree and validated before local integration or deployment.

**Tech Stack:** QMK/Vial C, Vial JSON, Kanata 1.12 configuration, Python validation tools, chezmoi, Claude agent markdown, Pi subagents.

## Global Constraints

- The saved Vial export remains canonical.
- FN2 top-left becomes transparent; do not restore `RGB_LYR` to the layout.
- Preserve all 31 currently ambiguous RGB classifications.
- Kanata uses exact QMK variants: `lmet` for the semicolon hold and `kp-` for W+E.
- Validate before deploying; do not flash firmware, push branches, or create releases.
- Keep one writer per repository and never edit a protected `main`/`master` checkout directly.

---

### Task 1: Synchronize firmware defaults and deterministic RGB clusters

**Files:**
- Modify: `keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c:90-379,556-562`
- Test: `tools/check_vil_keymap_sync.py`

**Interfaces:**
- Consumes: canonical `keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil`
- Produces: compiled defaults and deterministic RGB clusters aligned with that export

- [ ] **Step 1: Reproduce the static default failure**

Run from `/Users/brendan/code/iris-lm-config.fix-vial-sync-workflow`:

```bash
python3 tools/check_vil_keymap_sync.py
```

Expected: FAIL only for Layer 2 `_FN2`, slot 0: `RGB_LYR != KC_TRNS`.

- [ ] **Step 2: Make the saved Vial key authoritative in `_FN2`**

Change the first `_FN2` `LAYOUT(...)` argument:

```c
[_FN2] = LAYOUT(
KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_KP_8, KC_TRNS, KC_TRNS, KC_TRNS,
```

Keep the `RGB_LYR` custom keycode implementation available for future Vial assignment; only remove its current compiled mapping.

- [ ] **Step 3: Apply the deterministic RGB corrections**

Update comments to the current `.vil` keycode while changing only these cluster values:

```text
FN1: LED 46 _CS→_CC; 47 _CS→_CC
FN2: LED 0 _CK→_CN; 23 _CC→_CM; 25 _CC→_CM; 37 _CN→_CU;
     39 _CU→_CN; 55 _CA→_CM; 57 _CS→_CM; 59 _CS→_CM;
     60 _CV→_CM; 62 _CV→_CM
FN3: LED 34 _CN→_CK; 36 _CN→_CU; 37 _CK→_CU; 39 _CK→_CU;
     40 _CK→_CN; 42 _CK→_CN; 46 _CU→_CS; 51 _CU→_CS
FN4: LED 0 _CM→_CB
```

Do not change any other cluster entry. In particular, preserve the existing values for app shortcuts, bare-letter FN4 indicators, mouse keys, mouse buttons, `KC_ENTER`, and other ambiguous positions.

- [ ] **Step 4: Verify static defaults and compile**

```bash
python3 tools/check_vil_keymap_sync.py
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
```

Expected: drift checker prints `.vil/keymap.c static defaults are in sync`; QMK linking and BIN/HEX creation complete with `[OK]`.

- [ ] **Step 5: Verify the focused diff and commit**

```bash
git diff --check
git diff -- keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c
git add keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c
git commit -m "fix: sync Vial firmware defaults and RGB clusters"
```

Expected: one static keymap change plus exactly 21 RGB cluster changes and their comments.

---

### Task 2: Canonicalize and repair the keyboard agent definitions

**Files:**
- Create: `.claude/agents/keyb.vial-to-kanata.md`
- Modify: `.claude/agents/keyb.qmk-rgb.md`
- Modify: `README.md`
- Modify: `docs/workflow.md`
- Modify: `keyboards/keebio/iris_lm/keymaps/vial_custom/README.md`

**Interfaces:**
- Consumes: the canonical `.vil` and the existing Claude agent at `~/.claude/agents/keyb.vial-to-kanata.md`
- Produces: repository-versioned definitions suitable for both Claude and Pi discovery

- [ ] **Step 1: Copy the Kanata agent into the repository**

Copy `~/.claude/agents/keyb.vial-to-kanata.md` to `.claude/agents/keyb.vial-to-kanata.md` and add Pi-compatible skills frontmatter:

```yaml
skills: debugging-and-error-recovery,incremental-implementation,documentation-and-adrs,superpowers:verification-before-completion
```

- [ ] **Step 2: Replace cwd-dependent project paths**

Use these exact paths in the Kanata agent:

```markdown
- **Vial config (canonical):** `/Users/brendan/Code/iris-lm-config/keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil`
- **Kanata config (chezmoi source):** `/Users/brendan/.local/share/chezmoi/dot_config/kanata/config.kbd`
- **Kanata README (chezmoi source):** `/Users/brendan/.local/share/chezmoi/dot_config/kanata/README.md`
- **Deployed Kanata config:** `~/.config/kanata/config.kbd`
```

Replace every stale `dot_config/vial-qmk/keebio-iris-lm.vil` read instruction and make the validation command absolute.

- [ ] **Step 3: Correct exact-parity domain knowledge**

Apply these prompt corrections:

```markdown
| c | R3,C3 | `KC_KP_DOT` | `kp.` | Quantum |
| `KC_KP_MINUS` | `kp-` |
| `LGUI_T(KC_SCOLON)` | `(tap-hold $tap-timeout $hold-timeout ; lmet)` |
```

Document that Kanata must preserve keypad-vs-main-key and left-vs-right modifier variants when the installed Kanata supports them. `kp-` has been independently syntax-checked with Kanata 1.12.0.

- [ ] **Step 4: Make the RGB agent’s deterministic/ambiguous policy explicit**

Add a rule stating that an audit or sync may update automatic classifications only after presenting the diff, while existing values for unmatched keycodes remain unchanged unless the user explicitly classifies them. Do not change the classification outcome table selected in Task 1.

- [ ] **Step 5: Update workflow documentation**

Document both harness invocations:

```text
Claude: use the keyb:qmk-rgb agent / keyb:vial-to-kanata agent
Pi: /run keyb:qmk-rgb / /run keyb:vial-to-kanata
```

Replace the old path-A sequence with:

```text
Save Vial → run Kanata sync → run RGB sync → check_vil_keymap_sync.py → qmk compile
```

Remove documentation claiming `RGB_LYR` is currently mapped at FN2 top-left; clarify that `QK_KB_0` remains available but unassigned.

- [ ] **Step 6: Validate agent text and documentation**

```bash
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil >/dev/null
rg -n 'dot_config/vial-qmk|currently mapped on FN2|keyb:' .claude README.md docs keyboards/keebio/iris_lm/keymaps/vial_custom/README.md
git diff --check
```

Expected: no stale Vial source path or stale FN2 mapping claim; both agent names and Pi invocation are documented.

- [ ] **Step 7: Commit the agent and workflow repair**

```bash
git add .claude/agents README.md docs/workflow.md keyboards/keebio/iris_lm/keymaps/vial_custom/README.md
git commit -m "fix: repair keyboard sync agents"
```

- [ ] **Step 8: Integrate the verified repository branch locally**

After reviewing Tasks 1-2 and rerunning their verification commands:

```bash
wt -C /Users/brendan/code/iris-lm-config.fix-vial-sync-workflow merge --no-squash -y
```

Expected: local `main` fast-forwards and the implementation worktree is removed. Do not push.

---

### Task 3: Synchronize and deploy Kanata exact variants

**Files:**
- Modify in a dedicated chezmoi worktree: `dot_config/kanata/config.kbd:64-88`
- Modify in the same worktree: `dot_config/kanata/README.md`
- Deploy: `~/.config/kanata/config.kbd`

**Interfaces:**
- Consumes: QMK `LGUI_T(KC_SCOLON)` and `KC_KP_MINUS`
- Produces: validated/deployed Kanata equivalents `lmet` and `kp-`

- [ ] **Step 1: Create and baseline a chezmoi worktree**

```bash
cd /Users/brendan/.local/share/chezmoi
wt switch --create fix/vial-kanata-parity
cd /Users/brendan/.local/share/chezmoi.fix-vial-kanata-parity
kanata --check -c dot_config/kanata/config.kbd
```

Expected: `config file is valid` before edits.

- [ ] **Step 2: Apply exact QMK parity**

Change only these behavior lines:

```lisp
(w e)   kp-   $chord-timeout first-release ()  ;; we    = keypad -
```

```lisp
scln_cmd (tap-hold-release-keys $tap-timeout $hold-timeout ; lmet $right-hand-keys)  ;; ;: tap for ';', hold for left Command (Meta)
```

Update the README’s chord/homerow tables or prose to name keypad-minus and left Command exactly.

- [ ] **Step 3: Validate and commit the chezmoi source**

```bash
kanata --check -c dot_config/kanata/config.kbd
git diff --check
git diff -- dot_config/kanata/config.kbd dot_config/kanata/README.md
git add dot_config/kanata/config.kbd dot_config/kanata/README.md
git commit -m "fix: align Kanata with Vial key variants"
```

Expected: valid config and a two-behavior focused diff.

- [ ] **Step 4: Deploy from the validated worktree**

```bash
chezmoi --source /Users/brendan/.local/share/chezmoi.fix-vial-kanata-parity apply ~/.config/kanata/config.kbd
cmp /Users/brendan/.local/share/chezmoi.fix-vial-kanata-parity/dot_config/kanata/config.kbd ~/.config/kanata/config.kbd
sudo -n launchctl kickstart -k system/xbxd.kanata
```

Expected: `cmp` exits 0 and launchd restarts the service. If passwordless sudo is unavailable, stop and request the user’s interactive restart rather than hanging on a password prompt.

- [ ] **Step 5: Integrate the verified chezmoi branch locally**

```bash
wt -C /Users/brendan/.local/share/chezmoi.fix-vial-kanata-parity merge --no-squash -y
```

Expected: local chezmoi `main` fast-forwards and retains the validated source. Do not push.

---

### Task 4: Update live Claude/Pi discovery and Pi feature records

**Files:**
- Replace with symlink: `~/.claude/agents/keyb.vial-to-kanata.md`
- Modify in a dedicated `~/.pi/agent` worktree: `agents/keyb.vial-to-kanata.md`
- Modify: `docs/features/claude-agents-sync.md`
- Modify: `docs/index.json`
- Modify: `docs/changelog.md`

**Interfaces:**
- Consumes: repository canonical `.claude/agents/keyb.vial-to-kanata.md`
- Produces: Claude and Pi runtime discovery pointing to one versioned definition

- [ ] **Step 1: Ensure the canonical file exists at the stable main-checkout path**

Task 2 integrates the verified `iris-lm-config` branch before this task. Verify:

```bash
test -f /Users/brendan/Code/iris-lm-config/.claude/agents/keyb.vial-to-kanata.md
```

Do not push.

- [ ] **Step 2: Create a Pi config worktree**

```bash
cd /Users/brendan/.pi/agent
wt switch --create fix/keyb-agent-sync
cd /Users/brendan/.pi/agent.fix-keyb-agent-sync
```

- [ ] **Step 3: Replace Pi’s copied Kanata agent with the canonical symlink**

```bash
rm agents/keyb.vial-to-kanata.md
ln -s /Users/brendan/Code/iris-lm-config/.claude/agents/keyb.vial-to-kanata.md agents/keyb.vial-to-kanata.md
```

Do not change `agents/keyb.qmk-rgb.md`; it already points to the repository canonical agent.

- [ ] **Step 4: Update Pi durable feature documentation**

Update `docs/features/claude-agents-sync.md` to record both keyboard symlinks, their canonical repository, and the absolute Kanata/Vial path policy. Update `docs/index.json` entry `features.claude-agents-sync` with today’s modified date and the concrete agent path in `files_touched`. Add under the existing `2026-07-16` changelog section:

```markdown
- `claude-agents-sync` modified: Canonicalized both keyboard agents in `iris-lm-config`, repaired the Kanata agent’s Vial source path, and made Claude/Pi discovery share the versioned definitions.
  Files: `agents/keyb.qmk-rgb.md`, `agents/keyb.vial-to-kanata.md`, `docs/features/claude-agents-sync.md`, `docs/index.json`, `docs/changelog.md`
```

- [ ] **Step 5: Validate and commit Pi config documentation**

```bash
python3 scripts/validate-config-docs.py
git diff --check
git add agents/keyb.vial-to-kanata.md docs/features/claude-agents-sync.md docs/index.json docs/changelog.md
git commit -m "fix: canonicalize keyboard subagents"
```

Expected: config-doc validation passes.

- [ ] **Step 6: Integrate the Pi config branch locally and update Claude discovery**

After the Pi branch is verified, integrate it locally without pushing:

```bash
wt -C /Users/brendan/.pi/agent.fix-keyb-agent-sync merge --no-squash -y
```

Then replace Claude’s copied file with the same stable symlink:

```bash
backup="$HOME/.claude/agents/keyb.vial-to-kanata.md.bak-20260716"
cp "$HOME/.claude/agents/keyb.vial-to-kanata.md" "$backup"
rm "$HOME/.claude/agents/keyb.vial-to-kanata.md"
ln -s /Users/brendan/Code/iris-lm-config/.claude/agents/keyb.vial-to-kanata.md "$HOME/.claude/agents/keyb.vial-to-kanata.md"
```

Keep the backup until final discovery verification succeeds.

---

### Task 5: End-to-end verification and cleanup

**Files:**
- Verify only: both repositories, both discovery directories, deployed Kanata config

**Interfaces:**
- Consumes: Tasks 1-4
- Produces: evidence that the workflow is usable and clean

- [ ] **Step 1: Run firmware validation from the stable checkout**

```bash
cd /Users/brendan/Code/iris-lm-config
python3 -m json.tool qmk.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/vial.json >/dev/null
python3 -m json.tool keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil >/dev/null
python3 tools/check_vil_keymap_sync.py
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
```

Expected: all JSON parses, static defaults are in sync, and compile succeeds.

- [ ] **Step 2: Run Kanata and deployment validation**

```bash
kanata --check -c /Users/brendan/.local/share/chezmoi/dot_config/kanata/config.kbd
cmp /Users/brendan/.local/share/chezmoi/dot_config/kanata/config.kbd ~/.config/kanata/config.kbd
```

Expected: config valid and files identical after local branch integration.

- [ ] **Step 3: Verify agent discovery and shared content**

```bash
python3 - <<'PY'
from pathlib import Path
root = Path('/Users/brendan/Code/iris-lm-config/.claude/agents')
for live in [
    Path.home()/'.claude/agents/keyb.qmk-rgb.md',
    Path.home()/'.claude/agents/keyb.vial-to-kanata.md',
    Path.home()/'.pi/agent/agents/keyb.qmk-rgb.md',
    Path.home()/'.pi/agent/agents/keyb.vial-to-kanata.md',
]:
    assert live.is_symlink(), live
    assert live.resolve().parent == root, (live, live.resolve())
print('keyboard agent symlinks: OK')
PY
```

Run Pi agent discovery and confirm both `keyb:qmk-rgb` and `keyb:vial-to-kanata` appear as executable.

- [ ] **Step 4: Verify clean tracked state and report residual actions**

```bash
git -C /Users/brendan/Code/iris-lm-config status --short --branch
git -C /Users/brendan/.local/share/chezmoi status --short --branch
git -C /Users/brendan/.pi/agent status --short --branch
```

Expected: no unintended tracked or staged files. Report local commits/branches that remain unpushed and explicitly note that firmware was compiled but not flashed.
