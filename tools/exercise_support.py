"""Student selection helpers. Never rewrite a checked-in Renode script."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
PROJECTS = tuple(f"{n:02d}" for n in range(11))


def student_script(project: str, debug: bool = False) -> Path:
    if project not in PROJECTS:
        raise ValueError("Project must be 00..10")
    # Start from the regular script even for debug so CAN peers and overlays
    # are preserved. No emulation start before GDB has installed breakpoints.
    source = (ROOT / "renode" / f"project{project}.resc").read_text(encoding="utf-8")
    source = source.replace(f"@build/{project}/", f"@build/student/{project}/")
    if debug:
        source = re.sub(r"(?m)^start\s*$", "", source)
        source += '\nmach set 0\nmachine StartGdbServer 3333\n'
    build_dir = ROOT / "build" / "student" / project
    build_dir.mkdir(parents=True, exist_ok=True)
    path = build_dir / ("student-debug.resc" if debug else "student.resc")
    path.write_text(source, encoding="utf-8")
    return path
