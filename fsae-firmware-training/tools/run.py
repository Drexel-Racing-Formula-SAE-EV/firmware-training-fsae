#!/usr/bin/env python3
"""Launch one already-built project in Renode."""

from pathlib import Path
import argparse
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project", choices=[f"{n:02d}" for n in range(6)])
    args = parser.parse_args()
    elf = ROOT / "build" / args.project / f"project{args.project}.elf"
    if not elf.is_file():
        raise SystemExit(f"Build first: python tools/build.py {args.project}")
    script = ROOT / "renode" / f"project{args.project}.resc"
    return subprocess.run(["renode", str(script)], cwd=ROOT).returncode


if __name__ == "__main__":
    raise SystemExit(main())

