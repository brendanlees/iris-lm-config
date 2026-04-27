/* SPDX-License-Identifier: GPL-2.0-or-later */

#pragma once

// ============================================================================
// VIAL CONFIGURATION
// ============================================================================

#define VIAL_KEYBOARD_UID {0x97, 0xF0, 0xDF, 0xEC, 0x29, 0x72, 0x1E, 0x31}
#define VIAL_UNLOCK_COMBO_ROWS {0,9}
#define VIAL_UNLOCK_COMBO_COLS {0,5}

// Layer count: 7 (layers 0–6)
#define DYNAMIC_KEYMAP_LAYER_COUNT 7

// Vial tap dance (dynamic, configured via Vial GUI)
#define VIAL_TAP_DANCE_ENTRIES 32

// ============================================================================
// TAP AND HOLD / MOD-TAP
// ============================================================================

#define TAPPING_TERM 200

// ============================================================================
// RGB LAYER INDICATION
// ============================================================================

// Sync secondary (right) half layer state from primary (left) half
// Required for RGB indicators to work on both sides of split keyboard
#define SPLIT_LAYER_STATE_ENABLE
