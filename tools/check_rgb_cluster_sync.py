#!/usr/bin/env python3
"""Check deterministic RGB clusters against the canonical Vial export.

Unclassified keycodes are intentional user choices and are left unchanged. The
matrix-to-LED map is copied from the Iris LM K1 keyboard.json RGB layout so this
check does not require a QMK checkout.
"""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
KEYMAP_C = ROOT / "keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c"
VIL = ROOT / "keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil"

LED_BY_MATRIX = {
    (0, 0): 0, (0, 1): 2, (0, 2): 3, (0, 3): 5, (0, 4): 6, (0, 5): 8,
    (1, 0): 14, (1, 1): 13, (1, 2): 12, (1, 3): 11, (1, 4): 10, (1, 5): 9,
    (2, 0): 15, (2, 1): 16, (2, 2): 17, (2, 3): 18, (2, 4): 19, (2, 5): 20,
    (3, 0): 28, (3, 1): 26, (3, 2): 25, (3, 3): 23, (3, 4): 22, (3, 5): 21,
    (4, 2): 33, (4, 3): 32, (4, 4): 30, (4, 5): 29,
    (5, 0): 34, (5, 1): 36, (5, 2): 37, (5, 3): 39, (5, 4): 40, (5, 5): 42,
    (6, 0): 48, (6, 1): 47, (6, 2): 46, (6, 3): 45, (6, 4): 44, (6, 5): 43,
    (7, 0): 49, (7, 1): 50, (7, 2): 51, (7, 3): 52, (7, 4): 53, (7, 5): 54,
    (8, 0): 62, (8, 1): 60, (8, 2): 59, (8, 3): 57, (8, 4): 56, (8, 5): 55,
    (9, 2): 63, (9, 3): 64, (9, 4): 66, (9, 5): 67,
}
THUMB_LEDS = {29, 30, 32, 33, 63, 64, 66, 67}
TOGGLE_LAYERS = {4}


def normalize(keycode: str) -> str:
    aliases = {
        "KC_LBRC": "KC_LBRACKET",
        "KC_RBRC": "KC_RBRACKET",
        "KC_BSLS": "KC_BSLASH",
        "KC_PGDN": "KC_PGDOWN",
        "KC_NUM_LOCK": "KC_NUMLOCK",
        "KC_LCTL": "KC_LCTRL",
        "KC_RCTL": "KC_RCTRL",
    }
    value = re.sub(r"\s+", "", keycode)
    for old, new in aliases.items():
        value = re.sub(rf"\b{old}\b", new, value)
    return value


def classify(layer: int, led: int, keycode: str) -> str | None:
    keycode = normalize(keycode)

    if layer == 3 and led == 42 and keycode == "KC_NUMLOCK":
        return "_CB"  # Base animation while off; runtime code paints it red while on.
    if keycode in {"KC_TRNS", "KC_NO"}:
        if led in THUMB_LEDS or layer in TOGGLE_LAYERS:
            return "_CB"
        return "_CN"

    if re.fullmatch(r"KC_(?:[0-9]|KP_(?:[0-9]|ASTERISK|MINUS|SLASH|DOT))", keycode):
        return "_CU"
    if keycode in {
        "LSFT(KC_9)", "LSFT(KC_0)",
        "KC_LBRACKET", "KC_RBRACKET", "LSFT(KC_LBRACKET)",
        "LSFT(KC_RBRACKET)", "KC_BSLASH", "LSFT(KC_BSLASH)",
    }:
        return "_CC"
    if re.fullmatch(r"LSFT\(KC_[0-8]\)", keycode) or keycode in {
        "KC_MINUS", "KC_EQUAL", "LSFT(KC_EQUAL)", "KC_GRAVE", "LSFT(KC_GRAVE)",
    }:
        return "_CS"
    if keycode in {"KC_LEFT", "KC_RIGHT", "KC_UP", "KC_DOWN"}:
        return "_CA"
    if keycode in {
        "KC_MPLY", "KC_MUTE", "KC_VOLU", "KC_VOLD", "KC_MNXT", "KC_MPRV",
        "KC_BRIU", "KC_BRID", "KC_MFFD", "KC_MRWD", "KC_MSTP",
    }:
        return "_CM"
    if keycode in {"KC_PGUP", "KC_PGDOWN", "KC_HOME", "KC_END"}:
        return "_CV"
    if keycode in {
        "KC_TAB", "RSFT(KC_TAB)", "LALT(KC_TAB)", "LCTL(KC_TAB)",
        "QK_BOOT", "EE_CLR", "KC_NUMLOCK",
    } or keycode.startswith("TG("):
        return "_CK"
    if keycode.startswith("OSM(") or keycode.startswith("LSA(") or keycode in {
        "KC_LCTRL", "KC_RCTRL", "KC_LALT", "KC_RALT", "KC_LGUI", "KC_RGUI",
        "KC_LSHIFT", "KC_RSHIFT",
    } or re.match(r"^[LR](?:CTL|ALT|GUI|SFT)_T\(", keycode):
        return "_CO"

    # App shortcuts, mouse actions, plain letters on the HRM-off layer, Enter,
    # and other custom behavior retain their explicitly chosen current cluster.
    return None


def parse_clusters(text: str) -> dict[int, list[str]]:
    arrays: dict[int, list[str]] = {}
    pattern = re.compile(
        r"static const uint8_t fn(\d+)_clusters\[68\] PROGMEM = \{(.*?)\n\};",
        re.S,
    )
    for match in pattern.finditer(text):
        arrays[int(match.group(1))] = re.findall(r"\*/\s*(_C[A-Z]),", match.group(2))
    return arrays


def main() -> int:
    layouts = json.loads(VIL.read_text())["layout"]
    arrays = parse_clusters(KEYMAP_C.read_text())
    errors: list[str] = []

    for layer, current in sorted(arrays.items()):
        if len(current) != 68:
            errors.append(f"FN{layer}: cluster array has {len(current)} entries; expected 68")
            continue
        for (row, col), led in LED_BY_MATRIX.items():
            keycode = layouts[layer][row][col]
            if keycode == -1:
                continue
            expected = classify(layer, led, keycode)
            if expected is not None and current[led] != expected:
                errors.append(
                    f"FN{layer} LED {led} matrix [{row},{col}] {keycode}: "
                    f"{current[led]} != {expected}"
                )

    if errors:
        print("deterministic RGB cluster drift detected:")
        for error in errors:
            print(f"- {error}")
        return 1

    print("deterministic RGB clusters are in sync")
    return 0


if __name__ == "__main__":
    sys.exit(main())
