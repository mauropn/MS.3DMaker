#!/usr/bin/env python3
"""Run clang-format --dry-run -Werror on C++ sources (CI formatting gate)."""
from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GLOBS = ("src/**/*.cpp", "src/**/*.h", "tests/**/*.cpp", "tests/**/*.h")


def find_clang_format() -> str | None:
    for name in ("clang-format-18", "clang-format-17", "clang-format-16", "clang-format"):
        path = shutil.which(name)
        if path:
            return path
    # pip --user installs (Windows) often sit off PATH
    home = Path.home()
    roaming = home / "AppData" / "Roaming" / "Python"
    if roaming.is_dir():
        for scripts in sorted(roaming.glob("Python*/Scripts"), reverse=True):
            exe = scripts / "clang-format.exe"
            if exe.is_file():
                return str(exe)
    local = home / ".local" / "bin" / "clang-format"
    if local.is_file():
        return str(local)
    return None


def collect_files() -> list[Path]:
    files: list[Path] = []
    for pattern in GLOBS:
        files.extend(ROOT.glob(pattern))
    return sorted({p.resolve() for p in files if p.is_file()})


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--fix", action="store_true", help="Apply formatting instead of checking")
    args = parser.parse_args()

    clang_format = find_clang_format()
    if not clang_format:
        print("clang-format not found on PATH", file=sys.stderr)
        return 2

    files = collect_files()
    if not files:
        print("No C++ files found", file=sys.stderr)
        return 1

    style = ROOT / ".clang-format"
    if not style.is_file():
        print("Missing .clang-format", file=sys.stderr)
        return 1

    cmd = [clang_format, f"-style=file:{style}"]
    if args.fix:
        cmd.append("-i")
    else:
        cmd.extend(["--dry-run", "-Werror"])
    cmd.extend(str(p) for p in files)

    print(f"Using {clang_format} on {len(files)} file(s)")
    result = subprocess.run(cmd, cwd=ROOT)
    if result.returncode != 0 and not args.fix:
        print("Format check FAILED — run: python scripts/check_clang_format.py --fix", file=sys.stderr)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
