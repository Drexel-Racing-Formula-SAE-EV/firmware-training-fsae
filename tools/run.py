#!/usr/bin/env python3
"""Launch one already-built project in Renode."""

from pathlib import Path
import argparse
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project", choices=[f"{n:02d}" for n in range(11)])
    args = parser.parse_args()
    elf = ROOT / "build" / args.project / f"project{args.project}.elf"
    if not elf.is_file():
        raise SystemExit(f"Build first: python tools/build.py {args.project}")
    cache = ROOT / 'build' / args.project / 'CMakeCache.txt'
    if cache.is_file() and 'TRAINING_RENODE:BOOL=OFF' in cache.read_text(encoding='utf-8'):
        raise SystemExit(f'Hardware build selected. Rebuild for Renode: python tools/build.py {args.project} --clean')
    script = ROOT / "renode" / f"project{args.project}.resc"
    return subprocess.run(["renode", str(script)], cwd=ROOT).returncode


if __name__ == "__main__":
    raise SystemExit(main())
