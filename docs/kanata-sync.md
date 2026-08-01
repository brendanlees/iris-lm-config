# Kanata sync reference

Manual reference for mirroring the Iris Vial layout on the built-in laptop keyboard. Normally use the `vial-to-kanata` project skill.

## Sources

- Canonical Vial export: `keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil`
- Kanata chezmoi source: `~/.local/share/chezmoi/dot_config/kanata/config.kbd`
- Deployed Kanata config: `~/.config/kanata/config.kbd`

The sync is one-way from Vial to Kanata. Kanata timing and device filtering remain device-specific.

## Layer 1 mapping

Vial right-side rows are stored outer-to-inner at columns 0 through 5. Sanity-check `layout[0][7][5]` as `KC_H` before mapping.

| Laptop keys | Kanata outputs |
|---|---|
| `2 3` | `M-S-2 M-S-3` |
| `tab q w e r t` | `S-grv S-1 S-2 S-3 S-4 S-5` |
| `y u i o p` | `S-6 S-7 S-8 S-9 S-0` |
| `caps a s d f g` | `tab @osm_sg S-9 S-0 S-lbrc S-rbrc` |
| `h j k l ; '` | `left down up rght _ pgup` |
| `lsft z x c v b` | `S-tab lctl M-S-c kp. lbrc rbrc` |
| `n m , . / rsft` | `bsls S-bsls min S-eql eql pgdn` |

Kanata may render backslash as `\` and shifted backslash as `S-\`; validate the installed syntax rather than changing a working spelling.

## Base assignments and chords

Homerow holds must retain exact modifier sides:

| Key | Hold |
|---|---|
| `a` | `lmet` |
| `s` | `lalt` |
| `d` | `lsft` |
| `f` | `lctl` |
| `j` | `rctl` |
| `k` | `rsft` |
| `l` | `lalt` |
| `;` | `lmet` |

Active Vial combos map by their base-key identities:

| Keys | Output |
|---|---|
| `x c` | `S-min` |
| `w e` | `kp-` |
| `tab q` | `S-grv` |
| `q w` | `grv` |

Preserve keypad outputs such as `KC_KP_DOT` to `kp.` and `KC_KP_MINUS` to `kp-`.

## Manual validation and deployment

Make source changes only on an isolated chezmoi branch or worktree.

```sh
kanata --check -c ~/.local/share/chezmoi/dot_config/kanata/config.kbd
chezmoi apply ~/.config/kanata/config.kbd
cmp ~/.local/share/chezmoi/dot_config/kanata/config.kbd ~/.config/kanata/config.kbd
sudo launchctl kickstart -k system/xbxd.kanata
```

Validate before applying. Run the privileged restart interactively only after the deployed file matches the source.
