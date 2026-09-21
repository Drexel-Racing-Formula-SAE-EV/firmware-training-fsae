#!/usr/bin/env python3
"""Launch one already-built project in Renode."""

from pathlib import Path
import argparse
import subprocess
from exercise_support import student_script

ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project", choices=[f"{n:02d}" for n in range(11)])
    parser.add_argument("--student", action="store_true")
    args = parser.parse_args()
    build_dir = ROOT / "build" / "student" / args.project if args.student else ROOT / "build" / args.project
    suffix = " --student" if args.student else ""
    elf = build_dir / f"project{args.project}.elf"
    if not elf.is_file():
        raise SystemExit(f"Build first: python tools/build.py {args.project}{suffix}")
    cache = build_dir / 'CMakeCache.txt'
    if cache.is_file() and 'TRAINING_RENODE:BOOL=OFF' in cache.read_text(encoding='utf-8'):
        raise SystemExit(f'Hardware build selected. Rebuild for Renode: python tools/build.py {args.project}{suffix} --clean')
    script = ROOT / "renode" / f"project{args.project}.resc"
    if args.student:
        script = student_script(args.project)
    try:
        return subprocess.run(["renode", str(script)], cwd=ROOT).returncode
    except KeyboardInterrupt:
        print("\nRenode run interrupted.")
        return 130


if __name__ == "__main__":
    raise SystemExit(main())
