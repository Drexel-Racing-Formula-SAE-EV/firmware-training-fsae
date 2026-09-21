#!/usr/bin/env python3
"""Verify scaffold consistency, reference preservation and launcher isolation."""
import hashlib
import argparse
import json
from pathlib import Path
import re
import tempfile
import exercise_support

ROOT = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--built", action="store_true", help="Also audit all 22 existing ARM build selections")
    args = parser.parse_args()
    manifest = json.loads((ROOT / "exercises/manifest.json").read_text())
    assert set(manifest["projects"]) == set(exercise_support.PROJECTS)
    hashes = json.loads((ROOT / "exercises/reference-baseline.json").read_text())
    for path, expected in hashes.items():
        assert hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == expected, path
    for project, entry in manifest["projects"].items():
        base = ROOT / "exercises" / entry["directory"]
        assert (base / "README.md").is_file()
        assert (base / "wiring.md").is_file()
        for student, reference in entry["sources"].items():
            assert (ROOT / student).is_file(), student
            assert (ROOT / reference).is_file(), reference
            assert student.startswith("exercises/"), student
        assert len({t["id"] for t in entry["tasks"]}) == len(entry["tasks"])
        if args.built:
            for variant in ("reference", "student"):
                build = ROOT / "build" / project if variant == "reference" else ROOT / "build/student" / project
                database = json.loads((build / "compile_commands.json").read_text())
                compiled = {Path(item["file"]).resolve() for item in database}
                for student, reference in entry["sources"].items():
                    selected = reference if variant == "reference" else student
                    assert (ROOT / selected).resolve() in compiled, (variant, selected)
                if variant == "reference":
                    assert not any((ROOT / "exercises") in path.parents for path in compiled)
                else:
                    original_main = ROOT / "projects" / entry["directory"] / "src/main.c"
                    assert original_main.resolve() not in compiled
                if int(project) >= 6:
                    peer_map = build / "projects" / entry["directory"] / "training_can_peer.map"
                    assert "libtraining_student_common" not in peer_map.read_text()
                if variant == "student" and project in ("06", "09", "10"):
                    app_map = build / "projects" / entry["directory"] / f"project{project}.map"
                    assert "libtraining_student_common" in app_map.read_text()
    # Render in a disposable directory: never write or launch real user builds.
    with tempfile.TemporaryDirectory(prefix="fsae-script-check-") as temp:
        temp_root = Path(temp)
        (temp_root / "renode").mkdir()
        for project in exercise_support.PROJECTS:
            source = ROOT / "renode" / f"project{project}.resc"
            (temp_root / "renode" / source.name).write_bytes(source.read_bytes())
        original = exercise_support.ROOT
        try:
            exercise_support.ROOT = temp_root
            for project in exercise_support.PROJECTS:
                for debug in (False, True):
                    script = exercise_support.student_script(project, debug).read_text()
                    assert f"@build/student/{project}/project{project}.elf" in script
                    assert f"@build/{project}/" not in script
                    if int(project) >= 6:
                        assert f"@build/student/{project}/training_can_peer.elf" in script
                        assert script.count("connector Connect sysbus.can1 canHub") == 2
                    if project in ("02", "07", "08"):
                        assert "@renode/project02-button-exti.repl" in script
                    if debug:
                        assert not re.search(r"(?m)^start\s*$", script)
                        assert "mach set 0\nmachine StartGdbServer 3333" in script
                    else:
                        assert re.search(r"(?m)^start\s*$", script)
        finally:
            exercise_support.ROOT = original
    print(f"Scaffold: 11 mappings/labs, 22 launcher transformations and {len(hashes)} unchanged reference inputs PASS")
    print("This is structural validation, not a Renode execution test.")
    if args.built:
        print("All 22 build source selections and CAN peer link isolation: PASS")


if __name__ == "__main__":
    main()
