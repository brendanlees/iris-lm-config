# Iris LM Per-Layer RGB Implementation Plan

> **For agentic workers:** Use superpowers:subagent-driven-development or superpowers:executing-plans to implement task-by-task.

**Goal:** Rebuild vial_custom keymap with 7 layers, per-layer RGB indication using dynamic keycode checking (no hardcoded LED arrays), and clean up dead code.

**Architecture:** Single `keymap.c` file with embedded `rgb_matrix_indicators_advanced_user()` that iterates matrix positions, checks keycodes, and colours only non-transparent keys. Layer colours defined in a lookup table. Enable `SPLIT_LAYER_STATE_ENABLE` so both halves sync layer state. Remove separate RGB module, backup files, and commented-out tap dance C code.

**Tech Stack:** QMK, Vial, STM32G431, WS2812 RGB matrix (68 LEDs), split keyboard (USART)

---

## Task 1: Update config.h

**Files:**
- Modify: `keymaps/vial_custom/config.h`

- [ ] **Step 1: Back up current config.h**

```bash
cd /Users/brendan/Dev/keyboards/vial/vial-qmk/keyboards/keebio/iris_lm/keymaps/vial_custom
cp config.h config.h.old
```

- [ ] **Step 2: Replace config.h with minimal version**

```c
/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

// ============================================================================
// VIAL CONFIGURATION
// ============================================================================

#define VIAL_KEYBOARD_UID {0x97, 0xF0, 0xDF, 0xEC, 0x29, 0x72, 0x1E, 0x31}
#define VIAL_UNLOCK_COMBO_ROWS {0, 9}
#define VIAL_UNLOCK_COMBO_COLS {0, 5}

// Layer count: 7 (layers 0–6)
#define DYNAMIC_KEYMAP_LAYER_COUNT 7

// Vial tap dance (dynamic, configured via Vial GUI)
#define VIAL_TAP_DANCE_ENTRIES 32

// ============================================================================
// TAP AND HOLD / MOD-TAP
// ============================================================================

#define TAPPING_TERM 200
#define TAPPING_TERM_PER_KEY

// ============================================================================
// RGB LAYER INDICATION
// ============================================================================

// Sync secondary (right) half layer state from primary (left) half
// Required for RGB indicators to work on both sides of split keyboard
#define SPLIT_LAYER_STATE_ENABLE
```

- [ ] **Step 3: Verify syntax**

No errors expected. Check file was written:

```bash
cat config.h | head -20
```

Expected: First 20 lines show VIAL and layer count sections.

- [ ] **Step 4: Commit**

```bash
git add config.h
git commit -m "refactor: simplify vial_custom config for 7 layers"
```

---

## Task 2: Update rules.mk

**Files:**
- Modify: `keymaps/vial_custom/rules.mk`

- [ ] **Step 1: Replace rules.mk**

```makefile
VIA_ENABLE = yes
VIAL_ENABLE = yes
VIALRGB_ENABLE = yes
```

- [ ] **Step 2: Verify syntax**

```bash
cat rules.mk
```

Expected: Three lines, no `SRC += rgb_layers.c`.

- [ ] **Step 3: Commit**

```bash
git add rules.mk
git commit -m "refactor: remove rgb_layers module reference from rules.mk"
```

---

## Task 3: Rebuild keymap.c with 7 layers and embedded RGB indicators

**Files:**
- Modify: `keymaps/vial_custom/keymap.c`

This is the largest task. Replace the entire file with a version that:
1. Defines 7 layers (_MAIN through _FN6)
2. Preserves the existing keymaps for layers 0–3
3. Adds stub (all KC_TRNS) keymaps for layers 4–6
4. Embeds `rgb_matrix_indicators_advanced_user()` with dynamic keycode checking
5. Includes RGB_LYR toggle keycode
6. Removes old tap dance C functions (keep TD references in keymap)

- [ ] **Step 1: Back up current keymap.c**

```bash
cp keymap.c keymap.c.old
```

- [ ] **Step 2: Write new keymap.c**

```c
// Copyright 2025 Keebio (@keebio)
// SPDX-License-Identifier: GPL-2.0-or-later
// Per-layer RGB indication using dynamic keycode checking

#include QMK_KEYBOARD_H

// ============================================================================
// LAYER DEFINITIONS
// ============================================================================

enum custom_layer {
    _MAIN = 0,
    _FN1  = 1,
    _FN2  = 2,
    _FN3  = 3,
    _FN4  = 4,
    _FN5  = 5,
    _FN6  = 6,
};

// ============================================================================
// CUSTOM KEYCODES
// ============================================================================

enum custom_keycodes {
    RGB_LYR = QK_KB_0, // Toggle RGB layer indication on/off
};

// ============================================================================
// TAP DANCE CODES (Vial-managed, no C functions needed)
// ============================================================================

enum tap_dance_codes {
    TD_ESC_CTRL = 0,
    TD_TAB_L2   = 1,
};

// ============================================================================
// RGB LAYER COLOUR TABLE (HSV: Hue, Saturation=255, Value=dynamic)
// ============================================================================

static const uint8_t layer_colors[][3] = {
    [_MAIN] = {0,   0,   0},    // Layer 0: no colour override (use animation)
    [_FN1]  = {85,  255, 0},    // Layer 1: Green
    [_FN2]  = {128, 255, 0},    // Layer 2: Cyan
    [_FN3]  = {0,   255, 0},    // Layer 3: Red
    [_FN4]  = {170, 255, 0},    // Layer 4: Blue
    [_FN5]  = {21,  255, 0},    // Layer 5: Orange
    [_FN6]  = {213, 255, 0},    // Layer 6: Purple
};

static bool rgb_layer_indication_enabled = true;

// ============================================================================
// RGB MATRIX INDICATORS (Dynamic Keycode Checking)
// ============================================================================

#ifdef RGB_MATRIX_ENABLE

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    // Exit if indicator is disabled via RGB_LYR toggle
    if (!rgb_layer_indication_enabled) {
        return false;
    }

    uint8_t layer = get_highest_layer(layer_state);

    // Layer 0 (_MAIN) has no colour override, uses base animation
    if (layer == _MAIN) {
        return false;
    }

    // Get the layer's colour (HSV format)
    uint8_t hue = layer_colors[layer][0];
    uint8_t sat = layer_colors[layer][1];
    uint8_t val = rgb_matrix_get_val(); // Use current brightness from user settings

    // Convert HSV to RGB
    HSV hsv = {hue, sat, val};
    RGB rgb = hsv_to_rgb(hsv);

    // Iterate all matrix positions and colour non-transparent keys on this layer
    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            // Get the LED index for this matrix position
            uint8_t led_index = g_led_config.matrix_co[row][col];

            // Skip if no LED at this position or outside current batch range
            if (led_index == NO_LED || led_index < led_min || led_index >= led_max) {
                continue;
            }

            // Get the keycode at this position on the active layer
            uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){col, row});

            // Only colour if keycode is not transparent (KC_TRNS == 1)
            if (keycode > KC_TRNS) {
                rgb_matrix_set_color(led_index, rgb.r, rgb.g, rgb.b);
            }
        }
    }

    return false;
}

#endif // RGB_MATRIX_ENABLE

// ============================================================================
// KEYMAP (7 layers)
// ============================================================================

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Layer 0: _MAIN (QWERTY)
    [_MAIN] = LAYOUT(
        KC_EQUAL, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINUS,
        TD(TD_TAB_L2), KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        TD(TD_ESC_CTRL), LGUI_T(KC_A), LALT_T(KC_S), LSFT_T(KC_D), LCTL_T(KC_F), KC_G, KC_H, RCTL_T(KC_J), RSFT_T(KC_K), RALT_T(KC_L), RGUI_T(KC_SCLN), KC_QUOT,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_HOME, KC_END, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT,
        KC_LGUI, MO(1), KC_ENT, KC_SPC, LGUI(KC_ENT), KC_RALT
    ),

    // Layer 1: _FN1 (Numbers, symbols, navigation)
    [_FN1] = LAYOUT(
        KC_TILD, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PGUP,
        KC_GRV, _______, KC_UP, _______, QK_BOOT, _______, _______, KC_P7, KC_P8, KC_P9, KC_P0, KC_PGDN,
        KC_DEL, KC_LEFT, KC_DOWN, KC_RGHT, _______, KC_LBRC, KC_RBRC, KC_P4, KC_P5, KC_P6, KC_PLUS, KC_PIPE,
        RGB_MOD, _______, _______, _______, _______, KC_LCBR, KC_LPRN, KC_RPRN, KC_RCBR, KC_P1, KC_P2, KC_P3, KC_MINS, _______,
        _______, _______, KC_DEL, KC_DEL, _______, KC_P0
    ),

    // Layer 2: _FN2 (Function keys, RGB controls, media)
    [_FN2] = LAYOUT(
        KC_F12, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11,
        RGB_TOG, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, QK_BOOT,
        RGB_MOD, KC_MPRV, KC_MNXT, KC_VOLU, KC_PGUP, KC_UNDS, KC_EQL, KC_HOME, RGB_HUI, RGB_SAI, RGB_VAI, KC_BSLS,
        KC_MUTE, KC_MSTP, KC_MPLY, KC_VOLD, KC_PGDN, KC_MINS, KC_LPRN, _______, KC_PLUS, KC_END, RGB_HUD, RGB_SAD, RGB_VAD, _______,
        _______, _______, _______, _______, _______, _______
    ),

    // Layer 3: _FN3 (Admin, EE_CLR)
    [_FN3] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        EE_CLR, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, EE_CLR,
        _______, _______, _______, _______, _______, _______
    ),

    // Layer 4: _FN4 (Stub - all KC_TRNS)
    [_FN4] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),

    // Layer 5: _FN5 (Stub - all KC_TRNS)
    [_FN5] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),

    // Layer 6: _FN6 (Stub - all KC_TRNS)
    [_FN6] = LAYOUT(
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______
    ),
};

// ============================================================================
// CUSTOM KEYCODE HANDLING
// ============================================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RGB_LYR:
            if (record->event.pressed) {
                // Toggle RGB layer indication on/off
                rgb_layer_indication_enabled = !rgb_layer_indication_enabled;
            }
            return false;
        default:
            break;
    }
    return true;
}
```

- [ ] **Step 3: Verify keymap compiles**

```bash
cd /Users/brendan/Dev/keyboards/vial/vial-qmk
qmk compile -kb keebio/iris_lm/k1 -km vial_custom 2>&1 | head -50
```

Expected: No errors. Output should show compilation progressing.

- [ ] **Step 4: Commit**

```bash
git add keymaps/vial_custom/keymap.c
git commit -m "feat: rebuild keymap with 7 layers and dynamic RGB indicators"
```

---

## Task 4: Delete old RGB module files and backups

**Files:**
- Delete: `rgb_layers.c`, `rgb_layers.h`, and all `.backup` files

- [ ] **Step 1: Verify files to delete**

```bash
cd /Users/brendan/Dev/keyboards/vial/vial-qmk/keyboards/keebio/iris_lm/keymaps/vial_custom
ls -la | grep -E "rgb_layers|backup"
```

Expected: Shows `rgb_layers.c`, `rgb_layers.h`, `rgb_layers.c.approach1.backup`, `rgb_layers.c.fullversion.backup`, `rules.mk.vialrgb.backup`.

- [ ] **Step 2: Delete files**

```bash
rm -f rgb_layers.c rgb_layers.h rgb_layers.c.approach1.backup rgb_layers.c.fullversion.backup rules.mk.vialrgb.backup
```

- [ ] **Step 3: Verify deletion**

```bash
ls -la | grep -E "rgb_layers|backup"
```

Expected: No output (files deleted).

- [ ] **Step 4: Commit**

```bash
git add -u keymaps/vial_custom/
git commit -m "chore: remove old rgb_layers module and backup files"
```

---

## Task 5: Compile and verify on target hardware

**Files:**
- No files modified in this task; verification only

- [ ] **Step 1: Full clean compile**

```bash
cd /Users/brendan/Dev/keyboards/vial/vial-qmk
qmk compile -kb keebio/iris_lm/k1 -km vial_custom
```

Expected: Compilation completes successfully with a `.uf2` or `.bin` file output (path printed at end).

- [ ] **Step 2: Flash to keyboard (if DFU mode available)**

If you have the keyboard in bootloader mode:

```bash
# Example for STM32DFU (adjust path based on compile output)
dfu-util -a 0 --dfuse-address 0x08000000 -D build/keebio_iris_lm_k1_vial_custom.bin
```

Or use your flashing tool of choice. Expected: Keyboard reboots and functions normally.

- [ ] **Step 3: Test in Vial GUI**

1. Connect keyboard to Vial GUI
2. Open "Layers" panel — should show 7 layers (0–6)
3. Press layer selector to switch between layers
4. Verify RGB LEDs change colour per layer:
   - Layer 0 (_MAIN): Normal animation (no colour override)
   - Layer 1 (_FN1): All configured keys turn **green**
   - Layer 2 (_FN2): All configured keys turn **cyan**
   - Layer 3 (_FN3): Only `EE_CLR` keys turn **red**
   - Layers 4–6: Press keys to verify they light up with their assigned colours (blue, orange, purple)
5. Test `RGB_LYR` toggle — press the key to enable/disable layer indication
6. Test dynamic detection: Remap a key in Vial GUI on layer 2, switch to that layer, verify the new key gets the cyan colour

- [ ] **Step 4: Test secondary (right) half**

1. Check that secondary half displays the same layer colours as primary half
2. Verify `SPLIT_LAYER_STATE_ENABLE` is working (both halves sync layer state)

Expected: Both halves show identical colours, no sync delay.

- [ ] **Step 5: Quick regression test**

- Tap dance keys work (TD(0), TD(1))
- Home row mods respond correctly (LGUI_T, LALT_T, etc.)
- RGB brightness/hue/saturation controls in Vial GUI still work
- No visual glitches or flickering when switching layers

No commit needed for this task (verification only). If any issues found, they are addressed in subsequent tasks.
