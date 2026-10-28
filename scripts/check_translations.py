#!/usr/bin/env python3
"""Fail if translation catalogs have unfinished or empty user-facing strings."""
from __future__ import annotations

import argparse
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_TS = [
    ROOT / "translations" / "MS3DMaker_en.ts",
    ROOT / "translations" / "MS3DMaker_pt_BR.ts",
]


def check_ts(path: Path) -> list[str]:
    errors: list[str] = []
    tree = ET.parse(path)
    root = tree.getroot()
    for context in root.findall("context"):
        ctx_name = context.findtext("name") or "?"
        for message in context.findall("message"):
            source = (message.findtext("source") or "").strip()
            if not source:
                continue
            translation = message.find("translation")
            if translation is None:
                errors.append(f"{path.name}: [{ctx_name}] missing <translation> for {source!r}")
                continue
            typ = translation.get("type", "")
            text = (translation.text or "").strip()
            if typ == "unfinished":
                errors.append(f"{path.name}: [{ctx_name}] unfinished: {source!r}")
            elif typ == "vanished":
                continue
            elif not text and typ != "obsolete":
                # Allow empty only for intentionally empty sources (rare)
                if source:
                    errors.append(f"{path.name}: [{ctx_name}] empty translation: {source!r}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("ts_files", nargs="*", type=Path, default=DEFAULT_TS)
    args = parser.parse_args()
    all_errors: list[str] = []
    for path in args.ts_files:
        if not path.is_file():
            all_errors.append(f"missing catalog: {path}")
            continue
        all_errors.extend(check_ts(path))
    if all_errors:
        print("Translation check FAILED:", file=sys.stderr)
        for err in all_errors:
            print(f"  - {err}", file=sys.stderr)
        return 1
    print(f"Translation check OK ({len(args.ts_files)} catalog(s))")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
