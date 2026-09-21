#!/usr/bin/env python3
"""Run repository checks that are available on the current host."""

from pathlib import Path
import argparse
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def host_sensor_test() -> None:
    compiler = shutil.which("gcc") or shutil.which("clang")
    if compiler is None:
        raise RuntimeError("gcc or clang is required for host-side tests")
    with tempfile.TemporaryDirectory() as temp:
        executable = Path(temp) / "test_temperature"
        subprocess.run([
            compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
            "-I", str(ROOT / "projects/05_adc_sensor/include"),
            str(ROOT / "projects/05_adc_sensor/src/temperature_monitor.c"),
            str(ROOT / "tests/test_temperature_monitor.c"),
            "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project", choices=[f"{n:02d}" for n in range(6)])
    args = parser.parse_args()
    if args.project == "05":
        host_sensor_test()
    subprocess.run([sys.executable, str(ROOT / "tools/check_repository.py")], check=True)
    print(f"[PASS] Project {args.project} checks")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
