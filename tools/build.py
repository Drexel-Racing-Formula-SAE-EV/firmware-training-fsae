#!/usr/bin/env python3
"""Configure and build one training project."""

from pathlib import Path
import argparse
import shutil
import subprocess
import sys

PROJECTS = {f"{number:02d}" for number in range(11)}
ROOT = Path(__file__).resolve().parents[1]


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project", choices=sorted(PROJECTS))
    parser.add_argument("--clean", action="store_true")
    parser.add_argument("--hardware", action="store_true",
                        help="Use real ADC1 instead of the injected Renode backend")
    args = parser.parse_args()

    build_dir = ROOT / "build" / args.project
    if args.clean and build_dir.exists():
        shutil.rmtree(build_dir)

    configure = [
        "cmake", "-S", str(ROOT), "-B", str(build_dir), "-G", "Ninja",
        f"-DCMAKE_TOOLCHAIN_FILE={ROOT / 'cmake' / 'arm-none-eabi.cmake'}",
        f"-DTRAINING_PROJECT={args.project}",
        "-DCMAKE_BUILD_TYPE=Debug",
        f"-DTRAINING_RENODE={'OFF' if args.hardware else 'ON'}",
    ]
    subprocess.run(configure, check=True)
    subprocess.run(["cmake", "--build", str(build_dir)], check=True)

    elf = build_dir / f"project{args.project}.elf"
    if not elf.is_file():
        print(f"ERROR: expected ELF was not produced: {elf}", file=sys.stderr)
        return 1
    subprocess.run([sys.executable, str(ROOT / 'tools/verify_elf.py'), str(elf)], check=True)
    if int(args.project) >= 6:
        peer = build_dir / "training_can_peer.elf"
        if not peer.is_file():
            print(f"ERROR: expected CAN peer ELF was not produced: {peer}", file=sys.stderr)
            return 1
        subprocess.run([sys.executable, str(ROOT / 'tools/verify_elf.py'), str(peer)], check=True)
    # Record dependencies used; this is provenance, not an invented version pin.
    with (build_dir / 'dependencies.txt').open('w', encoding='utf-8') as report:
        for command in (["arm-none-eabi-gcc", "--version"],
                        ["git", "-C", str(ROOT / 'third_party/STM32CubeF4'), "rev-parse", "HEAD"],
                        ["git", "-C", str(ROOT / 'third_party/STM32CubeF4'), "submodule", "status", "--recursive"]):
            result = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            report.write(' '.join(command) + '\n' + result.stdout + '\n')
        if int(args.project) >= 7:
            command = ["git", "-C", str(ROOT / 'third_party/FreeRTOS-Kernel'),
                       "describe", "--tags", "--exact-match"]
            result = subprocess.run(command, text=True, stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT)
            report.write(' '.join(command) + '\n' + result.stdout + '\n')
    print(f"[PASS] {elf}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
