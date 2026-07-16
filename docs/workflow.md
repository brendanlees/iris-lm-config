# workflow

day-to-day cycle for editing, building, flashing, and shipping firmware from this repo.

## mental model

| repo | purpose | when to commit |
|---|---|---|
| `iris-lm-config` (this one) | **your code** — keymap, docs, tools | every meaningful keymap change |
| `vial-qmk` (sibling clone) | **build toolchain** — vial-kb's fork of qmk | never. just `git pull` periodically |

two repos, two histories. the only link between them is `qmk config user.overlay_dir`, which persists across sessions in qmk-cli's config file (see "qmk-cli config location" below).

## phase 1 — one-time github remote setup

pick a name and visibility, then:

```sh
cd <this-repo>
gh repo create <user>/iris-lm-config --private --source=. --push
git push --tags                 # send firmware-v1 tag too
```

`--source=.` uses the current local repo; `--push` sets `origin` and pushes `main` in one shot. swap `--private` for `--public` if you want it open. after this, `git remote -v` will show `origin` pointing at github.

## phase 2 — day-to-day edit cycle

```sh
cd <this-repo>

# 1. edit the keymap (whichever file)
$EDITOR keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c

# 2. build — qmk picks up the userspace automatically from anywhere
qmk compile -kb keebio/iris_lm/k1 -km vial_custom

# 3. flash the produced .bin to the keyboard
#    (qmk toolbox, or `qmk flash -kb keebio/iris_lm/k1 -km vial_custom` if you have bootloader perms)

# 4. test on the keyboard. if happy → commit
git add keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c
git commit -m "feat: bump fn3 numpad saturation"

# 5. push
git push
```

if the change touches multiple files (e.g. `config.h` or new docs), stage them individually rather than `git add -A`.

## phase 3 — releases

when you ship a known-good firmware, tag it:

```sh
git tag firmware-v2 -m "fn3 numpad polish"
git push origin firmware-v2     # or `git push --tags` to push all tags
```

build artifacts (`.bin`) are gitignored on purpose — releases are reproducible from the tag (`git checkout firmware-v2 && qmk compile ...`). if you want the binary attached to a github release page:

```sh
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
gh release create firmware-v2 keebio_iris_lm_k1_vial_custom.bin --notes "fn3 numpad polish"
```

## phase 4 — after a change (decision tree)

you changed something. which path?

**A) vial gui (any key remap)**
1. vial: **file → save current layout** (default path is `~/.config/vial-qmk/keebio-iris-lm.vil`, a symlink → repo file; writes land in the repo automatically)
2. claude: use the `keyb:qmk-rgb` agent / `keyb:vial-to-kanata` agent
3. Pi: `/run keyb:qmk-rgb` / `/run keyb:vial-to-kanata`
4. `python3 tools/check_vil_keymap_sync.py` — fails if the `.vil` layout and compiled defaults drift
5. `qmk compile -kb keebio/iris_lm/k1 -km vial_custom`
6. flash the `.bin`, test on the keyboard
7. `git add keyboards/... && git commit && git push`

**B) keymap.c only (no vial gui change)**
1. `python3 tools/check_vil_keymap_sync.py` if you touched static `LAYOUT(...)` defaults
2. `qmk compile -kb keebio/iris_lm/k1 -km vial_custom`
3. flash, test, commit, push
   (skip the agent — nothing in `keebio-iris-lm.vil` changed.)

**C) both** — treat as A.

**D) vial qmk settings tab (tap-hold tuning — chordal hold, flow tap, tapping term, permissive hold, …)**
1. tweak in vial → **QMK Settings** tab. change applies instantly via eeprom write over usb.
2. vial: **file → save current layout** — persists the new values into `keebio-iris-lm.vil`'s `settings` block so the repo stays in sync with the keyboard.
3. test on the keyboard.
4. `git add keyboards/.../keebio-iris-lm.vil && git commit && git push`. *no compile, no flash, no agent.*

   see [`docs/homerow-mods.md`](homerow-mods.md) for the canonical tap-hold values and the runtime-overrides-compile-time pitfall.

mental model: *touched vial keymap? save `.vil`, run Kanata sync, run RGB sync, run the sync check, then compile. touched vial qmk settings? just save the layout. touched `keymap.c` defaults? run the sync check, then compile.*

note on drift: when you remap inside the vial gui app, changes write directly to the keyboard's flash — they don't touch disk until step 1. until you save it, `.vil` drift on disk vs. on keyboard is invisible. worth a habit.

## keyb:qmk-rgb agent

claude code agent that keeps the per-layer RGB cluster arrays in `keymap.c` in sync with `keebio-iris-lm.vil`. lives at `.claude/agents/keyb.qmk-rgb.md` in this repo (symlinked to `~/.claude/agents/` for global access).

**when to invoke:** any time you change the keymap in vial gui (phase 4 path A or C above). skip it for pure `keymap.c` edits (path B).

**how to invoke (claude code):**

```
> use the keyb:qmk-rgb agent
```

**what it does:** reads `keebio-iris-lm.vil`, classifies each keycode into a color cluster (numbers, symbols, arrows, modifiers, …), diffs against the current `fn*_clusters` PROGMEM arrays in `keymap.c`, asks you to classify anything ambiguous, then updates the arrays (via Serena) and compiles to verify.

**what it does NOT do:** edit the keymap itself, flash the keyboard, or commit. those stay on you.

**you'll be asked to decide:** any keycode not in its built-in classification rules (custom keycodes, app-shortcut combos like `LALT(KC_3)`, screenshot shortcuts like `SGUI(KC_4)`, etc.). pick a cluster from the table in the agent file and it'll remember for that run.

**output:** per-layer diff of changed LED positions, list of ambiguous keycodes you classified, compile result. review, then commit if happy.

## keyb:vial-to-kanata agent

claude code agent that keeps the kanata config in sync with the canonical Vial export. versioned in this repo at `.claude/agents/keyb.vial-to-kanata.md`; `~/.claude/agents/keyb.vial-to-kanata.md` is a symlink to it.

**when to invoke:** any time you change the keymap in vial gui (phase 4 path A or C above), before or alongside the RGB sync.

**how to invoke (claude code):**

```
> use the keyb:vial-to-kanata agent
```

**how to invoke (Pi):**

```
/run keyb:vial-to-kanata
```

## phase 5 — pulling upstream vial-qmk updates

`vial-qmk` is now a clean tracker. when new vial-qmk releases land:

```sh
cd <vial-qmk>
git pull --ff-only origin vial    # always fast-forwards now, no rebase pain
```

then rebuild your firmware (`qmk compile ...`) to pick up upstream fixes. if a build breaks, that's an upstream regression — easy to diagnose because *your* code didn't change.

## qmk-cli config location

qmk-cli persists its settings (`user.qmk_home`, `user.overlay_dir`, etc.) to a platform-specific path. always use `qmk config <key>=<value>` to edit it — that way you don't need to remember the path. but if you ever need to inspect it directly:

| platform | path |
|---|---|
| macOS | `~/Library/Application Support/qmk/qmk.ini` |
| linux / xdg | `~/.config/qmk/qmk.ini` |
| windows | `%APPDATA%\qmk\qmk.ini` |

`qmk env` shows the active values (`QMK_HOME`, `QMK_FIRMWARE`, `QMK_USERSPACE`). `qmk userspace-doctor` should report `Userspace enabled: True`.

required values for this repo:
- `user.qmk_home` → path to your `vial-qmk` clone (e.g. `~/Code/vial-qmk`)
- `user.overlay_dir` → path to this repo (e.g. `~/Code/iris-lm-config`)

if you move either repo or set up a new machine, re-run:

```sh
qmk config user.qmk_home="$HOME/Code/vial-qmk" user.overlay_dir="$(realpath .)"   # from this repo's root
```

heads-up: an older or stale install may leave a second `qmk.ini` at the *other* path. qmk-cli only reads the platform-native one; the other becomes a misleading ghost. if `qmk config` and a `cat ~/.config/qmk/qmk.ini` disagree, that's the cause.

## archive of the old fork history

before this repo existed, ~23 commits lived on a local `vial-qmk` fork branch. those have been preserved at `<your-fork>/vial-qmk@archive/pre-userspace` on github (and locally as the `archive/pre-userspace` tag inside `vial-qmk`). reach for that if you need to dig up an old commit message or revert reasoning.
