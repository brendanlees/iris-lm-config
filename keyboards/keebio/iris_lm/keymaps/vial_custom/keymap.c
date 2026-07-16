// Copyright 2025 Keebio (@keebio)
// SPDX-License-Identifier: GPL-2.0-or-later
// Per-layer RGB indication using static LED cluster tables
// See .claude/agents/keyb.qmk-rgb.md for the LED index → matrix → key reference

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
// KEY CLUSTER DEFINITIONS (Tokyo Night Palette)
// ============================================================================

enum key_cluster {
    CLUSTER_NONE = 0,
    CLUSTER_NUMBERS,      // Yellow  — number keys, numpad, operators
    CLUSTER_SYMBOLS,      // Orange  — shifted symbols (! @ # $ % ^ & * ( ) - +)
    CLUSTER_CODE_SYMBOLS, // Cyan    — brackets, braces, pipes, chord keys
    CLUSTER_ARROWS,       // Green   — arrow keys (HJKL)
    CLUSTER_MEDIA,        // Red     — media controls
    CLUSTER_MODIFIERS,    // Purple  — modifier combos, app switcher
    CLUSTER_SPECIAL,      // Pink    — tab navigation (forward/back)
    CLUSTER_NAVIGATION,   // Sky     — page up/down
    CLUSTER_BASE,         // Pass-through — let base animation show (used for thumb keys)
};

// Cluster colors in HSV (Tokyo Night theme, hue 0-255, full saturation)
static const uint8_t cluster_colors[][3] = {
    [CLUSTER_NONE]         = {0,   0,   0},   // No color
    [CLUSTER_NUMBERS]      = {28,  255, 255}, // Yellow   #e0af68
    [CLUSTER_SYMBOLS]      = {12,  255, 255}, // Orange   #ff9e64
    [CLUSTER_CODE_SYMBOLS] = {154, 255, 255}, // Cyan     #7aa2f7
    [CLUSTER_ARROWS]       = {72,  255, 255}, // Green    #9ece6a
    [CLUSTER_MEDIA]        = {0,   255, 255}, // Red
    [CLUSTER_MODIFIERS]    = {188, 255, 255}, // Purple   #bb9af7
    [CLUSTER_SPECIAL]      = {213, 255, 255}, // Pink     #ff007c
    [CLUSTER_NAVIGATION]   = {141, 200, 255}, // Sky blue #7dcfff
    [CLUSTER_BASE]         = {0,   0,   0},   // Unused — indicator skips this LED
};

static bool rgb_layer_indication_enabled = true;

// ============================================================================
// STATIC PER-LAYER LED CLUSTER TABLES
// Each array is indexed by LED index (0-67) from keyboard.json rgb_matrix.layout.
// See .claude/agents/keyb.qmk-rgb.md for the full LED → matrix → key reference.
// ============================================================================

// Shorthand aliases for readability
#define _CN CLUSTER_NONE
#define _CS CLUSTER_SYMBOLS
#define _CC CLUSTER_CODE_SYMBOLS
#define _CA CLUSTER_ARROWS
#define _CM CLUSTER_MEDIA
#define _CO CLUSTER_MODIFIERS
#define _CK CLUSTER_SPECIAL
#define _CV CLUSTER_NAVIGATION
#define _CU CLUSTER_NUMBERS
#define _CB CLUSTER_BASE

// _FN1: Symbols, brackets, arrows, tab navigation
static const uint8_t fn1_clusters[68] PROGMEM = {
 /* 0  [0,0] ESC    KC_TRNS     */ _CN,
 /* 1  underglow                */ _CN,
 /* 2  [0,1] 1      KC_TRNS     */ _CN,
 /* 3  [0,2] 2      SGUI(KC_2)  */ _CO,
 /* 4  underglow                */ _CN,
 /* 5  [0,3] 3      SGUI(KC_3)  */ _CO,
 /* 6  [0,4] 4      KC_TRNS     */ _CN,
 /* 7  underglow                */ _CN,
 /* 8  [0,5] 5      KC_TRNS     */ _CN,
 /* 9  [1,5] %      LSFT(5)     */ _CS,
 /* 10 [1,4] $      LSFT(4)     */ _CS,
 /* 11 [1,3] #      LSFT(3)     */ _CS,
 /* 12 [1,2] @      LSFT(2)     */ _CS,
 /* 13 [1,1] !      LSFT(1)     */ _CS,
 /* 14 [1,0] ~      LSFT(GRV)   */ _CS,
 /* 15 [2,0] Tab    KC_TAB      */ _CK,
 /* 16 [2,1] Mod    OSM(SG)     */ _CO,
 /* 17 [2,2] (      LSFT(9)     */ _CC,
 /* 18 [2,3] )      LSFT(0)     */ _CC,
 /* 19 [2,4] {      LSFT(LBRC)  */ _CC,
 /* 20 [2,5] }      LSFT(RBRC)  */ _CC,
 /* 21 [3,5] ]      KC_RBRC     */ _CC,
 /* 22 [3,4] [      KC_LBRC     */ _CC,
 /* 23 [3,3] C+Spc  KC_KP_DOT   */ _CU,
 /* 24 underglow                */ _CN,
 /* 25 [3,2] X      SGUI(KC_C)  */ _CM,
 /* 26 [3,1] Z      KC_LCTL    */ _CO,
 /* 27 underglow                */ _CN,
 /* 28 [3,0] S+Tab  RSFT(TAB)   */ _CK,
 /* 29 [4,5] thumb  KC_NO       */ _CB,
 /* 30 [4,4] thumb  KC_TRNS     */ _CB,
 /* 31 underglow                */ _CN,
 /* 32 [4,3] thumb  KC_TRNS     */ _CB,
 /* 33 [4,2] thumb  KC_TRNS     */ _CB,
 /* 34 [5,0] BSPC   KC_TRNS     */ _CN,
 /* 35 underglow                */ _CN,
 /* 36 [5,1] 0      KC_TRNS     */ _CN,
 /* 37 [5,2] 9      KC_TRNS     */ _CN,
 /* 38 underglow                */ _CN,
 /* 39 [5,3] 8      KC_TRNS     */ _CN,
 /* 40 [5,4] 7      KC_TRNS     */ _CN,
 /* 41 underglow                */ _CN,
 /* 42 [5,5] 6      KC_TRNS     */ _CN,
 /* 43 [6,0] Y      ^  LSFT(6)   */ _CS,  // physical chain: outer→inner
 /* 44 [6,1] U      &  LSFT(7)   */ _CS,
 /* 45 [6,2] I      *  LSFT(8)   */ _CS,
 /* 46 [6,3] O      (  LSFT(9)   */ _CC,
 /* 47 [6,4] P      )  LSFT(0)   */ _CC,
 /* 48 [6,5] BSPC   KC_TRNS      */ _CN,
 /* 49 [7,5] '      KC_PGUP      */ _CV,  // physical chain: inner→outer
 /* 50 [7,4] ;      KC_TRNS      */ _CN,
 /* 51 [7,3] L      KC_RIGHT     */ _CA,
 /* 52 [7,2] K      KC_UP        */ _CA,
 /* 53 [7,1] J      KC_DOWN      */ _CA,
 /* 54 [7,0] H      KC_LEFT      */ _CA,
 /* 55 [8,0] N      \  KC_BSLS   */ _CC,  // physical chain: outer→inner
 /* 56 [8,1] M      |  LSFT(BSLS)*/ _CC,
 /* 57 [8,2] ,      -  KC_MINUS  */ _CS,
 /* 58 underglow                 */ _CN,
 /* 59 [8,3] .      +  LSFT(EQL) */ _CS,
 /* 60 [8,4] /      =  KC_EQUAL  */ _CS,
 /* 61 underglow                 */ _CN,
 /* 62 [8,5] RSht   KC_PGDN      */ _CV,
 /* 63 [9,2] thumb  KC_TRNS     */ _CB,
 /* 64 [9,3] thumb  KC_TRNS     */ _CB,
 /* 65 underglow                */ _CN,
 /* 66 [9,4] thumb  KC_TRNS     */ _CB,
 /* 67 [9,5] thumb  KC_NO       */ _CB,
};

// _FN2: Numpad, media controls, brightness
static const uint8_t fn2_clusters[68] PROGMEM = {
 /* 0  [0,0] KC_TRNS     */ _CN,
 /* 1  underglow         */ _CN,
 /* 2  [0,1] KC_TRNS     */ _CN,
 /* 3  [0,2] KC_TRNS     */ _CN,
 /* 4  underglow         */ _CN,
 /* 5  [0,3] KC_TRNS     */ _CN,
 /* 6  [0,4] KC_TRNS     */ _CN,
 /* 7  underglow         */ _CN,
 /* 8  [0,5] 5      KC_TRNS     */ _CN,
 /* 9  [1,5] KC_KP_5     */ _CU,
 /* 10 [1,4] KC_KP_4     */ _CU,
 /* 11 [1,3] KC_KP_3     */ _CU,
 /* 12 [1,2] KC_KP_2     */ _CU,
 /* 13 [1,1] KC_KP_1     */ _CU,
 /* 14 [1,0] KC_TRNS     */ _CN,
 /* 15 [2,0] KC_TAB      */ _CK,
 /* 16 [2,1] KC_TRNS     */ _CN,
 /* 17 [2,2] KC_TRNS     */ _CN,
 /* 18 [2,3] KC_TRNS     */ _CN,
 /* 19 [2,4] KC_TRNS     */ _CN,
 /* 20 [2,5] KC_TRNS     */ _CN,
 /* 21 [3,5] KC_TRNS     */ _CN,
 /* 22 [3,4] KC_TRNS     */ _CN,
 /* 23 [3,3] C      KC_BRIU     */ _CM,
 /* 24 underglow                */ _CN,
 /* 25 [3,2] X      KC_BRID     */ _CM,
 /* 26 [3,1] KC_TRNS     */ _CN,
 /* 27 underglow         */ _CN,
 /* 28 [3,0] KC_TRNS     */ _CN,
 /* 29 [4,5] thumb              */ _CB,
 /* 30 [4,4] thumb              */ _CB,
 /* 31 underglow                */ _CN,
 /* 32 [4,3] thumb              */ _CB,
 /* 33 [4,2] thumb              */ _CB,
 /* 34 [5,0] KC_TRNS     */ _CN,
 /* 35 underglow         */ _CN,
 /* 36 [5,1] KC_TRNS     */ _CN,
 /* 37 [5,2] 9      KC_TRNS     */ _CU,
 /* 38 underglow         */ _CN,
 /* 39 [5,3] 8      KC_KP_8     */ _CN,
 /* 40 [5,4] KC_TRNS     */ _CN,
 /* 41 underglow         */ _CN,
 /* 42 [5,5] KC_TRNS     */ _CN,
 /* 43 [6,0] Y      KC_KP_6     */ _CU,   // physical chain: outer→inner
 /* 44 [6,1] U      KC_KP_7     */ _CU,
 /* 45 [6,2] I      KC_KP_8     */ _CU,
 /* 46 [6,3] O      KC_KP_9     */ _CU,
 /* 47 [6,4] P      KC_KP_0     */ _CU,
 /* 48 [6,5] BSPC   KC_TRNS     */ _CN,
 /* 49 [7,5] '      KC_TRNS     */ _CN,   // physical chain: inner→outer
 /* 50 [7,4] ;      KC_TRNS     */ _CN,
 /* 51 [7,3] L      KC_TRNS     */ _CN,
 /* 52 [7,2] K      KC_TRNS     */ _CN,
 /* 53 [7,1] J      KC_TRNS     */ _CN,
 /* 54 [7,0] H      KC_TRNS     */ _CN,
 /* 55 [8,0] N      KC_MPLY     */ _CM,   // physical chain: outer→inner
 /* 56 [8,1] M      KC_MUTE     */ _CM,
 /* 57 [8,2] ,      KC_VOLD     */ _CM,
 /* 58 underglow                */ _CN,
 /* 59 [8,3] .      KC_VOLU     */ _CM,
 /* 60 [8,4] /      KC_MRWD     */ _CM,
 /* 61 underglow                */ _CN,
 /* 62 [8,5] RSht   KC_MFFD     */ _CM,
 /* 63 [9,2] thumb              */ _CB,
 /* 64 [9,3] thumb              */ _CB,
 /* 65 underglow                */ _CN,
 /* 66 [9,4] thumb              */ _CB,
 /* 67 [9,5] thumb              */ _CB,
};

// _FN3: App switcher + numpad overlay
// NumLock LED (LED 42, [5,5], 6-key, innermost) uses _CB + runtime red override when active (see indicator fn).
// Right thumb keys show base animation (_CB).
static const uint8_t fn3_clusters[68] PROGMEM = {
 /* 0  [0,0] KC_TRNS          */ _CN,
 /* 1  underglow               */ _CN,
 /* 2  [0,1] KC_TRNS          */ _CN,
 /* 3  [0,2] KC_TRNS          */ _CN,
 /* 4  underglow               */ _CN,
 /* 5  [0,3] KC_TRNS          */ _CN,
 /* 6  [0,4] KC_TRNS          */ _CN,
 /* 7  underglow               */ _CN,
 /* 8  [0,5] KC_TRNS          */ _CN,
 /* 9  [1,5] LALT(KC_5)       */ _CO,
 /* 10 [1,4] LALT(KC_4)       */ _CO,
 /* 11 [1,3] LALT(KC_3)       */ _CO,
 /* 12 [1,2] LALT(KC_2)       */ _CO,
 /* 13 [1,1] LALT(KC_1)       */ _CO,
 /* 14 [1,0] KC_TRNS          */ _CN,
 /* 15 [2,0] LALT(KC_TAB)     */ _CK,   // left caps → pink (tab function)
 /* 16 [2,1] A      LALT(KC_A)       */ _CK,
 /* 17 [2,2] S      LSA(KC_A)        */ _CO,
 /* 18 [2,3] D      KC_TRNS          */ _CN,
 /* 19 [2,4] F      KC_TRNS          */ _CN,
 /* 20 [2,5] KC_TRNS          */ _CN,
 /* 21 [3,5] KC_TRNS          */ _CN,
 /* 22 [3,4] KC_TRNS          */ _CN,
 /* 23 [3,3] KC_TRNS          */ _CN,
 /* 24 underglow               */ _CN,
 /* 25 [3,2] KC_TRNS          */ _CN,
 /* 26 [3,1] KC_TRNS          */ _CN,
 /* 27 underglow               */ _CN,
 /* 28 [3,0] LCTL(KC_TAB)     */ _CK,   // left shift → pink (tab function)
 /* 29 [4,5] thumb KC_TRNS    */ _CB,
 /* 30 [4,4] thumb KC_TRNS    */ _CB,
 /* 31 underglow               */ _CN,
 /* 32 [4,3] thumb KC_TRNS    */ _CB,
 /* 33 [4,2] thumb KC_TRNS    */ _CB,
 /* 34 [5,0] BSPC   KC_TRNS          */ _CK,   // physical chain: outer→inner
 /* 35 underglow               */ _CN,
 /* 36 [5,1] 0-key  KC_TRNS          */ _CU,
 /* 37 [5,2] 9-key  KC_KP_MINUS      */ _CU,
 /* 38 underglow               */ _CN,
 /* 39 [5,3] 8-key  KC_KP_ASTERISK   */ _CU,
 /* 40 [5,4] 7-key  KC_KP_SLASH      */ _CN,
 /* 41 underglow               */ _CN,
 /* 42 [5,5] 6-key  KC_NUM_LOCK       */ _CN,
 /* 43 [6,0] Y      KC_KP_7   */ _CU,   // physical chain: outer→inner
 /* 44 [6,1] U      KC_KP_8   */ _CU,
 /* 45 [6,2] I      KC_KP_9   */ _CU,
 /* 46 [6,3] O      LSFT(KC_EQL) */ _CS,   // numpad +
 /* 47 [6,4] P      KC_TRNS   */ _CN,
 /* 48 [6,5] BSPC   KC_TRNS   */ _CN,
 /* 49 [7,5] '      KC_TRNS   */ _CN,   // physical chain: inner→outer
 /* 50 [7,4] ;      KC_TRNS   */ _CN,
 /* 51 [7,3] L      LSFT(KC_EQL) */ _CS, // numpad +
 /* 52 [7,2] K      KC_KP_6   */ _CU,
 /* 53 [7,1] J      KC_KP_5   */ _CU,
 /* 54 [7,0] H      KC_KP_4   */ _CU,
 /* 55 [8,0] N      KC_KP_1   */ _CU,   // physical chain: outer→inner
 /* 56 [8,1] M      KC_KP_2   */ _CU,
 /* 57 [8,2] ,      KC_KP_3   */ _CU,
 /* 58 underglow               */ _CN,
 /* 59 [8,3] .      KC_ENTER     */ _CB, // enter → white (base animation)
 /* 60 [8,4] /      KC_TRNS   */ _CN,
 /* 61 underglow               */ _CN,
 /* 62 [8,5] RSht   KC_TRNS   */ _CN,
 /* 63 [9,2] thumb KC_KP_DOT   */ _CU,
 /* 64 [9,3] thumb KC_KP_0    */ _CU,
 /* 65 underglow               */ _CN,
 /* 66 [9,4] thumb KC_TRNS     */ _CB,
 /* 67 [9,5] thumb KC_TRNS     */ _CB,
};

// _FN4: Homerow mods off (toggle). ESC + homerow A–L = red; all others show base animation.
static const uint8_t fn4_clusters[68] PROGMEM = {
 /* 0  [0,0] ESC    TG(4)       */ _CB,   // layer-on indicator
 /* 1  underglow                */ _CN,
 /* 2  [0,1] 1      KC_TRNS     */ _CB,
 /* 3  [0,2] 2      KC_TRNS     */ _CB,
 /* 4  underglow                */ _CN,
 /* 5  [0,3] 3      KC_TRNS     */ _CB,
 /* 6  [0,4] 4      KC_TRNS     */ _CB,
 /* 7  underglow                */ _CN,
 /* 8  [0,5] 5      KC_TRNS     */ _CB,
 /* 9  [1,5] T      KC_TRNS     */ _CB,
 /* 10 [1,4] R      KC_TRNS     */ _CB,
 /* 11 [1,3] E      KC_TRNS     */ _CB,
 /* 12 [1,2] W      KC_TRNS     */ _CB,
 /* 13 [1,1] Q      KC_TRNS     */ _CB,
 /* 14 [1,0] Tab    KC_TRNS     */ _CB,
 /* 15 [2,0] LCtrl  KC_TRNS     */ _CB,
 /* 16 [2,1] A      KC_A        */ _CM,   // homerow
 /* 17 [2,2] S      KC_S        */ _CM,
 /* 18 [2,3] D      KC_D        */ _CM,
 /* 19 [2,4] F      KC_F        */ _CM,
 /* 20 [2,5] G      KC_G        */ _CM,
 /* 21 [3,5] B      KC_B        */ _CM,   // tap-dance disabled
 /* 22 [3,4] V      KC_V        */ _CM,
 /* 23 [3,3] C      KC_C        */ _CM,
 /* 24 underglow                */ _CN,
 /* 25 [3,2] X      KC_X        */ _CM,
 /* 26 [3,1] Z      KC_Z        */ _CM,
 /* 27 underglow                */ _CN,
 /* 28 [3,0] LSft   KC_TRNS     */ _CB,
 /* 29 [4,5] thumb  KC_TRNS     */ _CB,
 /* 30 [4,4] thumb  KC_TRNS     */ _CB,
 /* 31 underglow                */ _CN,
 /* 32 [4,3] thumb  KC_TRNS     */ _CB,
 /* 33 [4,2] thumb  KC_TRNS     */ _CB,
 /* 34 [5,0] BSPC   KC_TRNS     */ _CB,
 /* 35 underglow                */ _CN,
 /* 36 [5,1] 0      KC_TRNS     */ _CB,
 /* 37 [5,2] 9      KC_TRNS     */ _CB,
 /* 38 underglow                */ _CN,
 /* 39 [5,3] 8      KC_TRNS     */ _CB,
 /* 40 [5,4] 7      KC_TRNS     */ _CB,
 /* 41 underglow                */ _CN,
 /* 42 [5,5] 6      KC_TRNS     */ _CB,
 /* 43 [6,0] Y      KC_TRNS     */ _CB,   // physical: outer→inner
 /* 44 [6,1] U      KC_TRNS     */ _CB,
 /* 45 [6,2] I      KC_TRNS     */ _CB,
 /* 46 [6,3] O      KC_TRNS     */ _CB,
 /* 47 [6,4] P      KC_TRNS     */ _CB,
 /* 48 [6,5] BSPC   KC_TRNS     */ _CB,
 /* 49 [7,5] '      KC_TRNS     */ _CB,   // physical: inner→outer
 /* 50 [7,4] ;      KC_SCLN   */ _CB,
 /* 51 [7,3] L      KC_L        */ _CM,   // homerow
 /* 52 [7,2] K      KC_K        */ _CM,
 /* 53 [7,1] J      KC_J        */ _CM,
 /* 54 [7,0] H      KC_H        */ _CM,
 /* 55 [8,0] N      KC_TRNS     */ _CB,   // physical: outer→inner
 /* 56 [8,1] M      KC_TRNS     */ _CB,
 /* 57 [8,2] ,      KC_TRNS     */ _CB,
 /* 58 underglow                */ _CN,
 /* 59 [8,3] .      KC_TRNS     */ _CB,
 /* 60 [8,4] /      KC_TRNS     */ _CB,
 /* 61 underglow                */ _CN,
 /* 62 [8,5] RSft   KC_TRNS     */ _CB,
 /* 63 [9,2] thumb  KC_TRNS     */ _CB,
 /* 64 [9,3] thumb  KC_TRNS     */ _CB,
 /* 65 underglow                */ _CN,
 /* 66 [9,4] thumb  KC_TRNS     */ _CB,
 /* 67 [9,5] thumb  KC_TRNS     */ _CB,
};

// _FN5: Mouse layer (activated via TD(1) = MO(5)). Mouse arrows green, wheel sky, buttons thumb pass-through.
static const uint8_t fn5_clusters[68] PROGMEM = {
 /* 0  [0,0] ESC    KC_TRNS     */ _CN,
 /* 1  underglow                */ _CN,
 /* 2  [0,1] 1      KC_TRNS     */ _CN,
 /* 3  [0,2] 2      KC_TRNS     */ _CN,
 /* 4  underglow                */ _CN,
 /* 5  [0,3] 3      KC_TRNS     */ _CN,
 /* 6  [0,4] 4      KC_TRNS     */ _CN,
 /* 7  underglow                */ _CN,
 /* 8  [0,5] 5      KC_TRNS     */ _CN,
 /* 9  [1,5] T      KC_TRNS     */ _CN,
 /* 10 [1,4] R      KC_TRNS     */ _CN,
 /* 11 [1,3] E      KC_TRNS     */ _CN,
 /* 12 [1,2] W      KC_TRNS     */ _CN,
 /* 13 [1,1] Q      KC_TRNS     */ _CN,
 /* 14 [1,0] Tab    KC_TRNS     */ _CN,
 /* 15 [2,0] LCtrl  KC_TRNS     */ _CN,
 /* 16 [2,1] A      KC_TRNS     */ _CN,
 /* 17 [2,2] S      KC_TRNS     */ _CN,
 /* 18 [2,3] D      KC_TRNS     */ _CN,
 /* 19 [2,4] F      KC_TRNS     */ _CN,
 /* 20 [2,5] G      KC_TRNS     */ _CN,
 /* 21 [3,5] B      KC_TRNS     */ _CN,
 /* 22 [3,4] V      KC_TRNS     */ _CN,
 /* 23 [3,3] C      KC_TRNS     */ _CN,
 /* 24 underglow                */ _CN,
 /* 25 [3,2] X      KC_TRNS     */ _CN,
 /* 26 [3,1] Z      KC_TRNS     */ _CN,
 /* 27 underglow                */ _CN,
 /* 28 [3,0] LSft   KC_TRNS     */ _CN,
 /* 29 [4,5] thumb  KC_TRNS     */ _CB,
 /* 30 [4,4] thumb  KC_TRNS     */ _CB,
 /* 31 underglow                */ _CN,
 /* 32 [4,3] thumb  KC_TRNS     */ _CB,
 /* 33 [4,2] thumb  KC_TRNS     */ _CB,
 /* 34 [5,0] BSPC   KC_TRNS     */ _CN,
 /* 35 underglow                */ _CN,
 /* 36 [5,1] 0      KC_TRNS     */ _CN,
 /* 37 [5,2] 9      KC_TRNS     */ _CN,
 /* 38 underglow                */ _CN,
 /* 39 [5,3] 8      KC_TRNS     */ _CN,
 /* 40 [5,4] 7      KC_TRNS     */ _CN,
 /* 41 underglow                */ _CN,
 /* 42 [5,5] 6      KC_TRNS     */ _CN,
 /* 43 [6,0] Y      KC_TRNS     */ _CN,
 /* 44 [6,1] U      KC_TRNS     */ _CN,
 /* 45 [6,2] I      KC_TRNS     */ _CN,
 /* 46 [6,3] O      KC_TRNS     */ _CN,
 /* 47 [6,4] P      KC_TRNS     */ _CN,
 /* 48 [6,5] BSPC   KC_TRNS     */ _CN,
 /* 49 [7,5] '      KC_WH_U     */ _CV,
 /* 50 [7,4] ;      KC_TRNS     */ _CN,
 /* 51 [7,3] L      KC_MS_R     */ _CA,
 /* 52 [7,2] K      KC_MS_U     */ _CA,
 /* 53 [7,1] J      KC_MS_D     */ _CA,
 /* 54 [7,0] H      KC_MS_L     */ _CA,
 /* 55 [8,0] N      KC_TRNS     */ _CN,
 /* 56 [8,1] M      KC_TRNS     */ _CN,
 /* 57 [8,2] ,      KC_TRNS     */ _CN,
 /* 58 underglow                */ _CN,
 /* 59 [8,3] .      KC_TRNS     */ _CN,
 /* 60 [8,4] /      KC_TRNS     */ _CN,
 /* 61 underglow                */ _CN,
 /* 62 [8,5] RSft   KC_WH_D     */ _CV,
 /* 63 [9,2] thumb  KC_TRNS     */ _CB,
 /* 64 [9,3] thumb  KC_BTN2     */ _CB,
 /* 65 underglow                */ _CN,
 /* 66 [9,4] thumb  KC_BTN1     */ _CB,
 /* 67 [9,5] thumb  KC_TRNS     */ _CB,
};

#undef _CN
#undef _CS
#undef _CC
#undef _CA
#undef _CM
#undef _CO
#undef _CK
#undef _CV
#undef _CU
#undef _CB

static const uint8_t *get_layer_clusters(uint8_t layer) {
    switch (layer) {
        case _FN1: return fn1_clusters;
        case _FN2: return fn2_clusters;
        case _FN3: return fn3_clusters;
        case _FN4: return fn4_clusters;
        case _FN5: return fn5_clusters;
        default:   return NULL;
    }
}

// ============================================================================
// RGB MATRIX INDICATORS (Static LED cluster table lookup)
// ============================================================================

#ifdef RGB_MATRIX_ENABLE

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    if (!rgb_layer_indication_enabled) {
        return false;
    }

    uint8_t layer = get_highest_layer(layer_state);

    // Layer 0 (_MAIN) has no colour override, uses base animation
    if (layer == _MAIN) {
        return false;
    }

    const uint8_t *clusters = get_layer_clusters(layer);
    if (clusters == NULL) {
        return false;  // FN4-FN6 are stubs — no coloring needed
    }

    uint8_t val = rgb_matrix_get_val();

    for (uint8_t i = led_min; i < led_max; i++) {
        uint8_t c = pgm_read_byte(&clusters[i]);
        if (c == CLUSTER_NONE) {
            rgb_matrix_set_color(i, 0, 0, 0);
        } else if (c != CLUSTER_BASE) {
            HSV hsv = {cluster_colors[c][0], cluster_colors[c][1], val};
            RGB rgb = hsv_to_rgb(hsv);
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
        // CLUSTER_BASE: skip — base animation shows through
    }

    // NumLock LED on FN3 (LED 42, [5,5], 6-key, innermost): red when active, base anim when off
    if (layer == _FN3 && 42 >= led_min && 42 < led_max) {
        if (host_keyboard_led_state().num_lock) {
            HSV hsv = {cluster_colors[CLUSTER_MEDIA][0], cluster_colors[CLUSTER_MEDIA][1], val};
            RGB rgb = hsv_to_rgb(hsv);
            rgb_matrix_set_color(42, rgb.r, rgb.g, rgb.b);
        }
    }

    // Layer 4 (no-homerow-mods) active: ESC (LED 0) glows red on all layers
    if (IS_LAYER_ON(_FN4) && 0 >= led_min && 0 < led_max) {
        HSV hsv = {cluster_colors[CLUSTER_MEDIA][0], cluster_colors[CLUSTER_MEDIA][1], val};
        RGB rgb = hsv_to_rgb(hsv);
        rgb_matrix_set_color(0, rgb.r, rgb.g, rgb.b);
    }

    return false;
}

#endif // RGB_MATRIX_ENABLE

// ============================================================================
// KEYMAP (7 layers)
// ============================================================================

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    // Layer 0: _MAIN (Base layer from Vial export)
    [_MAIN] = LAYOUT(
        TG(4), KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC,
        TD(1), KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        ALL_T(KC_ESCAPE), LGUI_T(KC_A), LALT_T(KC_S), LSFT_T(KC_D), LCTL_T(KC_F), KC_G, KC_H, RCTL_T(KC_J), RSFT_T(KC_K), LALT_T(KC_L), LGUI_T(KC_SCLN), KC_QUOTE,
        OSM(MOD_LSFT), TD(13), TD(12), TD(10), TD(11), TD(14), KC_NO, KC_NO, KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLASH, OSM(MOD_RSFT),
        LGUI_T(KC_BSPC), LT(1, KC_SPACE), LT(3, KC_ENTER), LT(3, KC_ENTER), LT(2, KC_SPACE), OSM(MOD_RSFT)
    ),

    // Layer 1: _FN1 (Symbols & navigation from Vial export)
    [_FN1] = LAYOUT(
        KC_TRNS, KC_TRNS, SGUI(KC_2), SGUI(KC_3), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        LSFT(KC_GRAVE), LSFT(KC_1), LSFT(KC_2), LSFT(KC_3), LSFT(KC_4), LSFT(KC_5), LSFT(KC_6), LSFT(KC_7), LSFT(KC_8), LSFT(KC_9), LSFT(KC_0), KC_TRNS,
        KC_TAB, OSM(MOD_LSFT|MOD_LGUI), LSFT(KC_9), LSFT(KC_0), LSFT(KC_LBRC), LSFT(KC_RBRC), KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, KC_TRNS, KC_PGUP,
        RSFT(KC_TAB), KC_LCTL, SGUI(KC_C), KC_KP_DOT, KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS, KC_BSLS, LSFT(KC_BSLS), KC_MINUS, LSFT(KC_EQUAL), KC_EQUAL, KC_PGDN,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    // Layer 2: _FN2 (Function keys, media & utilities from Vial export)
    [_FN2] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_KP_8, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_KP_1, KC_KP_2, KC_KP_3, KC_KP_4, KC_KP_5, KC_KP_6, KC_KP_7, KC_KP_8, KC_KP_9, KC_KP_0, KC_TRNS,
        KC_TAB, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_BRID, KC_BRIU, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MPLY, KC_MUTE, KC_VOLD, KC_VOLU, KC_MRWD, KC_MFFD,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    // Layer 3: _FN3 (App switcher & numpad from Vial export)
    [_FN3] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NUM_LOCK, KC_KP_SLASH, KC_KP_ASTERISK, KC_KP_MINUS, KC_TRNS, KC_TRNS,
        KC_TRNS, LALT(KC_1), LALT(KC_2), LALT(KC_3), LALT(KC_4), LALT(KC_5), KC_KP_7, KC_KP_8, KC_KP_9, LSFT(KC_EQUAL), KC_TRNS, KC_TRNS,
        LALT(KC_TAB), LALT(KC_A), LSA(KC_A), KC_TRNS, KC_TRNS, KC_TRNS, KC_KP_4, KC_KP_5, KC_KP_6, LSFT(KC_EQUAL), KC_TRNS, KC_TRNS,
        LCTL(KC_TAB), KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_KP_1, KC_KP_2, KC_KP_3, KC_ENTER, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_KP_0, KC_KP_DOT
    ),

    // Layer 4: _FN4 (Homerow-mods-off from Vial export)
    [_FN4] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_TRNS,
        KC_TRNS, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    // Layer 5: _FN5 (Mouse keys from Vial export)
    [_FN5] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MS_L, KC_MS_D, KC_MS_U, KC_MS_R, KC_TRNS, KC_WH_U,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_WH_D,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_BTN1, KC_BTN2, KC_TRNS
    ),

    // Layer 6: _FN6 (Reserved from Vial export)
    [_FN6] = LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
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
