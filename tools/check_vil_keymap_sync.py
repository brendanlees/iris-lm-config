#!/usr/bin/env python3
"""Check that QMK static defaults in keymap.c match the canonical Vial export.

This intentionally does not require qmk or vial-qmk to be installed. The
Keebio Iris LM K1 LAYOUT matrix order is copied from upstream keyboard.json so
CI can catch drift in a plain Python environment.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
KEYMAP_C = ROOT / "keyboards/keebio/iris_lm/keymaps/vial_custom/keymap.c"
VIL = ROOT / "keyboards/keebio/iris_lm/keymaps/vial_custom/keebio-iris-lm.vil"

LAYER_NAMES = ["_MAIN", "_FN1", "_FN2", "_FN3", "_FN4", "_FN5", "_FN6"]

# Copied from vial-qmk/keyboards/keebio/iris_lm/k1/keyboard.json
# layouts.LAYOUT.layout[*].matrix. Keep this local so syntax/drift checks do not
# require a full QMK checkout.
LAYOUT_MATRIX_ORDER: list[tuple[int, int]] = [
    (0, 0), (0, 1), (0, 2), (0, 3), (0, 4), (0, 5), (5, 5), (5, 4), (5, 3), (5, 2), (5, 1), (5, 0),
    (1, 0), (1, 1), (1, 2), (1, 3), (1, 4), (1, 5), (6, 5), (6, 4), (6, 3), (6, 2), (6, 1), (6, 0),
    (2, 0), (2, 1), (2, 2), (2, 3), (2, 4), (2, 5), (7, 5), (7, 4), (7, 3), (7, 2), (7, 1), (7, 0),
    (3, 0), (3, 1), (3, 2), (3, 3), (3, 4), (3, 5), (4, 5), (9, 5), (8, 5), (8, 4), (8, 3), (8, 2), (8, 1), (8, 0),
    (4, 2), (4, 3), (4, 4), (9, 4), (9, 3), (9, 2),
]

ALIASES = {
    "_______": "KC_TRNS",
    "XXXXXXX": "KC_NO",
    "QK_KB_0": "RGB_LYR",
    "KC_ESC": "KC_ESCAPE",
    "KC_BSPC": "KC_BSPACE",
    "KC_SCLN": "KC_SCOLON",
    "KC_LBRC": "KC_LBRACKET",
    "KC_RBRC": "KC_RBRACKET",
    "KC_BSLS": "KC_BSLASH",
    "KC_PGDN": "KC_PGDOWN",
    "KC_NUM_LOCK": "KC_NUMLOCK",
    "KC_NLCK": "KC_NUMLOCK",
    "KC_LCTL": "KC_LCTRL",
    "KC_RCTL": "KC_RCTRL",
}

INACTIVE = {"KC_TRNS", "KC_NO"}


def strip_c_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    text = re.sub(r"//.*", "", text)
    return text


def find_matching_paren(text: str, open_index: int) -> int:
    depth = 0
    for index in range(open_index, len(text)):
        char = text[index]
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return index
    raise ValueError("unclosed LAYOUT(...) block in keymap.c")


def split_top_level_commas(body: str) -> list[str]:
    items: list[str] = []
    start = 0
    depth = 0
    for index, char in enumerate(body):
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
        elif char == "," and depth == 0:
            items.append(body[start:index].strip())
            start = index + 1
    tail = body[start:].strip()
    if tail:
        items.append(tail)
    return items


def parse_keymap_layers(path: Path) -> dict[str, list[str]]:
    text = path.read_text()
    layers: dict[str, list[str]] = {}
    pattern = re.compile(r"\[(_[A-Z0-9]+)\]\s*=\s*LAYOUT\s*\(")
    for match in pattern.finditer(text):
        layer_name = match.group(1)
        open_index = match.end() - 1
        close_index = find_matching_paren(text, open_index)
        body = strip_c_comments(text[open_index + 1:close_index])
        layers[layer_name] = split_top_level_commas(body)
    return layers


def normalize_keycode(keycode: str) -> str:
    keycode = keycode.strip()
    keycode = re.sub(r"\bLT(\d)\(", r"LT(\1,", keycode)
    keycode = re.sub(r"\s+", "", keycode)

    def replace_token(match: re.Match[str]) -> str:
        token = match.group(0)
        return ALIASES.get(token, token)

    keycode = re.sub(r"\b[A-Z][A-Z0-9_]*\b", replace_token, keycode)
    return ALIASES.get(keycode, keycode)


def expected_layer_values(vil_layer: list[list[str]]) -> list[str]:
    return [vil_layer[row][col] for row, col in LAYOUT_MATRIX_ORDER]


def active_count(values: list[str]) -> int:
    return sum(1 for value in values if normalize_keycode(value) not in INACTIVE)


def check(vil_path: Path, keymap_path: Path) -> list[str]:
    errors: list[str] = []
    vil_data = json.loads(vil_path.read_text())
    vil_layers = vil_data.get("layout", [])
    c_layers = parse_keymap_layers(keymap_path)

    if len(vil_layers) != len(LAYER_NAMES):
        errors.append(f".vil has {len(vil_layers)} layers; expected {len(LAYER_NAMES)}")

    for layer_index, layer_name in enumerate(LAYER_NAMES):
        if layer_index >= len(vil_layers):
            continue
        expected = expected_layer_values(vil_layers[layer_index])
        actual = c_layers.get(layer_name)
        if actual is None:
            errors.append(f"Layer {layer_index} {layer_name}: missing from keymap.c")
            continue
        if len(actual) != len(LAYOUT_MATRIX_ORDER):
            errors.append(
                f"Layer {layer_index} {layer_name}: keymap.c has {len(actual)} LAYOUT args; "
                f"expected {len(LAYOUT_MATRIX_ORDER)}"
            )
            continue

        if active_count(expected) > 0 and all(normalize_keycode(value) == "KC_TRNS" for value in actual):
            errors.append(
                f"Layer {layer_index} {layer_name}: keymap.c is all-transparent but .vil has "
                f"{active_count(expected)} active keys"
            )

        for slot, (expected_key, actual_key) in enumerate(zip(expected, actual)):
            expected_normalized = normalize_keycode(expected_key)
            actual_normalized = normalize_keycode(actual_key)
            if expected_normalized != actual_normalized:
                row, col = LAYOUT_MATRIX_ORDER[slot]
                errors.append(
                    f"Layer {layer_index} {layer_name} slot {slot} matrix [{row},{col}]: "
                    f"keymap.c {actual_key} != .vil {expected_key}"
                )
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--vil", type=Path, default=VIL)
    parser.add_argument("--keymap", type=Path, default=KEYMAP_C)
    args = parser.parse_args()

    errors = check(args.vil, args.keymap)
    if errors:
        print(".vil/keymap.c drift detected:")
        for error in errors:
            print(f"- {error}")
        return 1

    print(".vil/keymap.c static defaults are in sync")
    return 0


if __name__ == "__main__":
    sys.exit(main())
