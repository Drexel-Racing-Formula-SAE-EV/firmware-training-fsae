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
        executable = Path(temp) / "test_temperature.exe"
        subprocess.run([
            compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
            "-I", str(ROOT / "projects/05_adc_sensor/include"),
            str(ROOT / "projects/05_adc_sensor/src/temperature_monitor.c"),
            str(ROOT / "tests/test_temperature_monitor.c"),
            "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)


def host_training_logic_test() -> None:
    compiler = shutil.which("gcc") or shutil.which("clang")
    if compiler is None:
        raise RuntimeError("gcc or clang is required for host-side tests")
    with tempfile.TemporaryDirectory() as temp:
        executable = Path(temp) / "test_training_logic.exe"
        subprocess.run([
            compiler, "-std=c11", "-Wall", "-Wextra", "-Werror",
            "-I", str(ROOT / "platform/stm32f4/include"),
            "-I", str(ROOT / "common/include"),
            str(ROOT / "common/src/training_can_protocol.c"),
            str(ROOT / "common/src/training_fault_manager.c"),
            str(ROOT / "common/src/training_ecu.c"),
            str(ROOT / "tests/test_training_logic.c"),
            "-o", str(executable),
        ], check=True)
        subprocess.run([str(executable)], check=True)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("project", choices=[f"{n:02d}" for n in range(11)])
    args = parser.parse_args()
    host_sensor_test()
    host_training_logic_test()
    compiler = shutil.which('gcc') or shutil.which('clang')
    with tempfile.TemporaryDirectory() as temp:
        executable = Path(temp) / 'test_platform.exe'
        subprocess.run([compiler, '-std=c11', '-Wall', '-Wextra', '-Werror',
                        '-DTRAINING_RENODE=1', '-I', str(ROOT / 'tests/stubs'),
                        '-I', str(ROOT / 'platform/stm32f4/include'),
                        str(ROOT / 'tests/test_platform_host.c'), '-o', str(executable)], check=True)
        subprocess.run([str(executable)], check=True)
    subprocess.run([sys.executable, str(ROOT / 'tests/test_verify_elf.py')], check=True)
    subprocess.run([sys.executable, str(ROOT / "tools/check_repository.py")], check=True)
    print(f"[PASS] Host regressions and structure for {args.project}; NOT an ARM/Renode execution test")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
