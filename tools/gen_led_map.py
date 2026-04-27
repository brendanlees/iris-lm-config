#!/usr/bin/env python3
"""
gen_led_map.py — Prints a table of LED index → matrix position → LAYOUT slot
for the Keebio Iris LM keyboard.

Usage: python3 tools/gen_led_map.py
Run from: keyboards/keebio/iris_lm/
"""

import json
import sys
from pathlib import Path


def main():
    kb_json = Path("k1/keyboard.json")
    if not kb_json.exists():
        sys.exit("Error: Run from keyboards/keebio/iris_lm/ (k1/keyboard.json not found)")

    data = json.loads(kb_json.read_text())

    # Build matrix → layout-slot index lookup
    layout_keys = data["layouts"]["LAYOUT"]["layout"]
    matrix_to_slot = {}
    for slot_idx, key in enumerate(layout_keys):
        if "matrix" in key:
            r, c = key["matrix"]
            matrix_to_slot[(r, c)] = slot_idx

    # Iterate LED layout
    led_layout = data["rgb_matrix"]["layout"]
    total = len(led_layout)

    print(f"{'LED':>4}  {'Type':6}  {'Matrix':8}  {'Slot':6}  {'x':6}  {'y':6}")
    print("-" * 50)
    for led_idx, entry in enumerate(led_layout):
        flags = entry.get("flags", 0)
        x = entry.get("x", 0)
        y = entry.get("y", 0)
        if flags == 2 or "matrix" not in entry:
            print(f"{led_idx:>4}  {'under':6}  {'--':8}  {'--':6}  {x:6.1f}  {y:6.1f}")
        else:
            r, c = entry["matrix"]
            slot = matrix_to_slot.get((r, c), "?")
            print(f"{led_idx:>4}  {'key':6}  [{r},{c}]     {str(slot):6}  {x:6.1f}  {y:6.1f}")

    print(f"\nTotal LEDs: {total} ({data['rgb_matrix']['split_count']} split)")


if __name__ == "__main__":
    main()
