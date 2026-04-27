# Cluster-Based RGB Indicators with Tokyo Night Theme

**Date:** 2026-04-10  
**Status:** Design Review

---

## Overview

Replace uniform layer-wide color with **cluster-based coloring** where keys are grouped by function (numbers, symbols, arrows, media, etc.). Each cluster gets its own Tokyo Night color. Base layer stays plain white (no override).

---

## Tokyo Night Color Palette (HSV)

| Cluster | Tokyo Night | Hex | HSV (H, S, V) | Purpose |
|---------|---|---|---|---|
| Numbers | Yellow | #e0af68 | 40, 138, 224 | Number keys, numpad |
| Symbols | Orange | #ff9e64 | 25, 153, 255 | Shifted symbols (!, @, #, etc.) |
| Code Symbols | Cyan | #7aa2f7 | 217, 128, 247 | Brackets, braces, parens |
| Arrows | Green | #9ece6a | 102, 148, 206 | Arrow keys (HJKL mapped) |
| Media | Red | #f7768e | 341, 115, 247 | Play, pause, volume, mute |
| Modifiers | Purple | #bb9af7 | 265, 66, 247 | Ctrl, Alt, Shift, Cmd |
| Special | Pink | #f7768e | 341, 115, 247 | QK_BOOT, EE_CLR, resets |
| Base Layer | White | #ffffff | 0, 0, 255 | No color override (use animation) |

---

## Cluster Assignments by Layer

### Layer 0: Base (_MAIN)
- **Numbers:** Top row (1-5, 6-0)
- **Modifiers:** Home row mods (Ctrl, Alt, Shift on ASDF / HJKL)
- **Special:** Shift keys (large keys at bottom)
- Rest: Transparent (use base animation)

### Layer 1: Symbols & Navigation (_FN1)
- **Symbols:** Top row (!, @, #, $, %, ^, &, *, (, ))
- **Code Symbols:** Brackets/braces ([, ], {, }, |, \)
- **Arrows:** HJKL (mapped to ↑, ↓, ←, →)
- **Modifiers:** Shift, Ctrl, Alt keys
- Rest: Transparent

### Layer 2: Function Keys & Media (_FN2)
- **Numpad:** Numpad numbers (0-9)
- **Media:** Play, pause, next, prev, volume up/down, mute
- **Special:** Brightness up/down, QK_BOOT
- Rest: Transparent

### Layer 3: App Switcher & Numpad (_FN3)
- **Modifiers:** Alt+number (app switcher)
- **Numpad:** Numpad layout (full grid on right side)
- **Special:** Num Lock, Enter
- Rest: Transparent

### Layers 4-6: Stubs
- All transparent (will inherit clustering once populated)

---

## Implementation Approach

**Cluster Lookup Table:**  
For each layer, define which keys belong to which cluster using a bitmask or enum-based approach.

**Per-Key Cluster Check:**  
In `rgb_matrix_indicators_advanced_user()`, instead of checking only `keycode > KC_TRNS`, also check the keycode against the cluster table to determine which color to apply.

**Code structure:**
```c
enum key_cluster {
    CLUSTER_NONE = 0,
    CLUSTER_NUMBERS,
    CLUSTER_SYMBOLS,
    CLUSTER_CODE_SYMBOLS,
    CLUSTER_ARROWS,
    CLUSTER_MEDIA,
    CLUSTER_MODIFIERS,
    CLUSTER_SPECIAL,
};

// Color table for clusters (HSV)
static const uint8_t cluster_colors[][3] = {
    [CLUSTER_NONE]         = {0,   0,   0},    // No color
    [CLUSTER_NUMBERS]      = {40,  138, 224},  // Yellow
    [CLUSTER_SYMBOLS]      = {25,  153, 255},  // Orange
    [CLUSTER_CODE_SYMBOLS] = {217, 128, 247},  // Cyan
    [CLUSTER_ARROWS]       = {102, 148, 206},  // Green
    [CLUSTER_MEDIA]        = {341, 115, 247},  // Red (note: >255 will wrap)
    [CLUSTER_MODIFIERS]    = {265, 66,  247},  // Purple
    [CLUSTER_SPECIAL]      = {341, 115, 247},  // Pink
};

// Per-layer cluster assignments (row × col)
static const enum key_cluster layer_clusters[DYNAMIC_KEYMAP_LAYER_COUNT][MATRIX_ROWS][MATRIX_COLS];
```

**Alternative approach (simpler):**  
Define cluster lookup as a 2D array per layer, where each array position contains the cluster enum for that key position. Simpler to maintain and visualize.

---

## Verification & Testing

1. **Visual:** Load each layer, verify colors match clusters (numbers = yellow, arrows = green, etc.)
2. **Toggle:** RGB_LYR key still toggles indication on/off
3. **Reconfig:** Change a key in Vial → switch layers → verify new key gets correct cluster color
4. **Right side:** Both halves show same cluster colors (SPLIT_LAYER_STATE_ENABLE still active)

---

## Design Tradeoffs

**Pros:**
- Visual function mapping (makes finding keys faster)
- Tokyo Night matches terminal aesthetic
- Maintainable (easy to update cluster logic)
- Falls back to layer color if a key is uncategorized

**Cons:**
- Slightly more code (~100-150 lines for cluster tables)
- Requires maintaining cluster assignments when remapping
- Per-key color lookup is marginally slower (but still negligible)

---

## Not In Scope

- Animation-based clustering (beyond static colors)
- Dynamic cluster detection (analyzing keymap at runtime to auto-assign clusters)
- Per-cluster configuration via Vial GUI (keep it firmware-based for now)

---

## Next Steps

1. User approval of colors and cluster assignments
2. Implement cluster lookup tables in keymap.c
3. Update `rgb_matrix_indicators_advanced_user()` to check clusters
4. Test all layers and verify colors match assignments
5. Recompile and flash both halves
