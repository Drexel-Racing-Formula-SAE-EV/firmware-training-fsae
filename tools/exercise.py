#!/usr/bin/env python3
"""Check STUDENT sources; reference test success is never student completion."""
import argparse
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
from exercise_support import PROJECTS, ROOT, student_script


def run_host(project, entry, reference):
    compiler = shutil.which("gcc") or shutil.which("clang")
    supported = {"00", "01", "02", "03", "04", "05", "06", "09", "10"}
    if project not in supported:
        print("No automated behavioral test for this lab; use its Renode/GDB acceptance checklist.")
        return True
    if compiler is None:
        print("BLOCKED: install native gcc or clang for host tests (Arm GCC cannot run host tests).")
        return False
    mapping = {ref: student for student, ref in entry["sources"].items()}
    def src(ref):
        return str(ROOT / (ref if reference else mapping.get(ref, ref)))
    args = [compiler, "-std=c11", "-Wall", "-Wextra",
            "-I", str(ROOT / "platform/stm32f4/include"),
            "-I", str(ROOT / "common/include"),
            "-I", str(ROOT / "projects/05_adc_sensor/include")]
    if project in {"00", "02"}:
        main_path = Path(src(f"projects/{entry['directory']}/src/main.c")).as_posix()
        args += ["-I", str(ROOT / "tests/exercises/stubs"),
                 f'-DTRAINING_MAIN_SOURCE="{main_path}"']
        if project == "02":
            args += [src("platform/stm32f4/src/board_io.c")]
        fixture = "test_bringup.c" if project == "00" else "test_exti.c"
        args += [str(ROOT / "tests/exercises" / fixture)]
    elif project == "01":
        args += ["-I", str(ROOT / "tests/exercises/stubs"),
                 src("platform/stm32f4/src/board_io.c"),
                 str(ROOT / "tests/exercises/test_gpio.c")]
    elif project == "03":
        args += ["-I", str(ROOT / "tests/stubs"),
                 src("platform/stm32f4/src/timebase.c"),
                 str(ROOT / "tests/exercises/test_timebase.c")]
    elif project == "04":
        path = Path(src("platform/stm32f4/src/console.c")).as_posix()
        args += ["-DTRAINING_RENODE=1", f'-DTRAINING_CONSOLE_SOURCE="{path}"',
                 "-I", str(ROOT / "tests/stubs"),
                 str(ROOT / "tests/exercises/test_console.c")]
    elif project == "05":
        args += [src("projects/05_adc_sensor/src/temperature_monitor.c"),
                 str(ROOT / "tests/test_temperature_monitor.c")]
    else:
        args += [src("common/src/" + name + ".c") for name in
                 ("training_can_protocol", "training_fault_manager", "training_ecu")]
        args += [str(ROOT / "tests/exercises/test_logic.c")]
    with tempfile.TemporaryDirectory(prefix="fsae-exercise-") as temp:
        exe = Path(temp) / "check.exe"
        result = subprocess.run(args + ["-o", str(exe)])
        if result.returncode != 0:
            return False
        # Assertions in these binaries are real behavior checks, not TODO scans.
        try:
            result = subprocess.run([str(exe)], text=True, stdout=subprocess.PIPE,
                                    stderr=subprocess.STDOUT, timeout=10)
        except subprocess.TimeoutExpired:
            print("BEHAVIOR CHECK FAILED: test exceeded 10 seconds. Check for a loop that never yields or returns.")
            return False
        if result.returncode != 0:
            print("BEHAVIOR CHECK FAILED: an implemented function did not meet the test expectations.")
        if result.stdout:
            print(result.stdout.strip())
        return result.returncode == 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("action", choices=("check", "script"))
    parser.add_argument("project", choices=PROJECTS)
    parser.add_argument("--reference", action="store_true",
                        help="Check reference sources, for instructor test validation only")
    parser.add_argument("--debug", action="store_true")
    parser.add_argument("--test-incomplete", action="store_true",
                        help="Run behavioral tests even with remaining TODOs (for partial implementations)")
    args = parser.parse_args()
    if args.action == "script":
        print(student_script(args.project, args.debug))
        return 0
    manifest = json.loads((ROOT / "exercises/manifest.json").read_text(encoding="utf-8"))
    entry = manifest["projects"][args.project]
    incomplete = []
    if not args.reference:
        for path in entry["sources"]:
            for number, line in enumerate((ROOT / path).read_text(encoding="utf-8").splitlines(), 1):
                if re.search(r"TODO\(\d{2}\.\d+\)", line):
                    incomplete.append(f"{path}:{number}: {line.strip()}")
    print(f"Testing {'REFERENCE' if args.reference else 'STUDENT'} project {args.project}", flush=True)
    for line in incomplete:
        print("INCOMPLETE:", line)
    if incomplete and not args.test_incomplete:
        print("NOT YET IMPLEMENTED: this is expected for an untouched exercise, not a build failure.")
        print(f"Open exercises/{entry['directory']}/README.md and complete the listed TODOs.")
        print("Remove each TODO marker after implementing it, then rerun this command.")
        print("Behavior tests were not run. Use --test-incomplete to test partial work explicitly.")
        return 1
    passed = run_host(args.project, entry, args.reference)
    if incomplete or not passed:
        print("NOT COMPLETE: implement the tasks, then rerun. Do not remove markers before implementation.")
        return 1
    print("Available source/host checks passed; this does NOT certify Renode or hardware behavior.")
    print("Record every manual acceptance item in the lab README before declaring completion.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
