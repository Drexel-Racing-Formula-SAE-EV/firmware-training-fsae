#!/usr/bin/env python3
"""Configure and build one training project."""

from pathlib import Path
import argparse
import shutil
import subprocess
import sys

PROJECTS = {f"{number:02d}" for number in range(6)}
ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project", choices=sorted(PROJECTS))
    parser.add_argument("--clean", action="store_true")
    args = parser.parse_args()

    build_dir = ROOT / "build" / args.project
    if args.clean and build_dir.exists():
        shutil.rmtree(build_dir)

    configure = [
        "cmake", "-S", str(ROOT), "-B", str(build_dir), "-G", "Ninja",
        f"-DCMAKE_TOOLCHAIN_FILE={ROOT / 'cmake' / 'arm-none-eabi.cmake'}",
        f"-DTRAINING_PROJECT={args.project}",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DTRAINING_RENODE=ON",
    ]
    subprocess.run(configure, check=True)
    subprocess.run(["cmake", "--build", str(build_dir)], check=True)

    elf = build_dir / f"project{args.project}.elf"
    if not elf.is_file():
        print(f"ERROR: expected ELF was not produced: {elf}", file=sys.stderr)
        return 1
    print(f"[PASS] {elf}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

