# workflow

day-to-day cycle for editing, building, flashing, and shipping firmware from this repo.

## mental model

| repo | purpose | when to commit |
|---|---|---|
| `iris-lm-config` (this one) | **your code** — keymap, docs, tools | every meaningful keymap change |
| `vial-qmk` (sibling clone) | **build toolchain** — vial-kb's fork of qmk | never. just `git pull` periodically |

two repos, two histories. the only link between them is `qmk config user.overlay_dir`, which persists across sessions in `~/.config/qmk/qmk.ini`.

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

## phase 4 — capturing vial gui changes into git

when you remap layers inside the **vial gui app**, those changes write directly to the keyboard's flash — they don't touch disk. to capture that state into git:

1. in vial gui: **file → save current layout** → save *over* `keyboards/keebio/iris_lm/keymaps/vial_custom/vial.vil`
2. `git add ... && git commit -m "vial: rebind layer 2"` and push as usual

until you save it, vial.vil drift on disk vs. on keyboard is invisible — easy to forget, worth a habit.

## phase 5 — pulling upstream vial-qmk updates

`vial-qmk` is now a clean tracker. when new vial-qmk releases land:

```sh
cd <vial-qmk>
git pull --ff-only origin vial    # always fast-forwards now, no rebase pain
```

then rebuild your firmware (`qmk compile ...`) to pick up upstream fixes. if a build breaks, that's an upstream regression — easy to diagnose because *your* code didn't change.

## archive of the old fork history

before this repo existed, ~23 commits lived on a local `vial-qmk` fork branch. those have been preserved at `<your-fork>/vial-qmk@archive/pre-userspace` on github (and locally as the `archive/pre-userspace` tag inside `vial-qmk`). reach for that if you need to dig up an old commit message or revert reasoning.
