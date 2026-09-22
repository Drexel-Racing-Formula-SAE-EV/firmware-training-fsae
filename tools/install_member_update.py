#!/usr/bin/env python3
"""Update member docs and helper scripts without overwriting student C code."""
import argparse
from datetime import datetime
from pathlib import Path
import shutil
import uuid

OBSOLETE = (
    "docs/ROADMAP.md", "docs/VALIDATION.md",
    "docs/STUDENT_LABS_VALIDATION.md", "exercises/INSTRUCTOR.md",
)


def install(package, repo):
    package, repo = Path(package).resolve(), Path(repo).resolve()
    if repo == package:
        raise ValueError("Run this from an extracted update package, targeting your existing repository.")
    for marker in ("CMakeLists.txt", "exercises/manifest.json"):
        if not (repo / marker).is_file():
            raise ValueError(f"Target must already contain the v2.0 student track: missing {marker}")
    updates = [
        "README.md", "tools/exercise.py", "tools/run.py",
        "tools/install_member_update.py", "tests/test_member_workflow.py",
    ]
    for folder in ("docs", "exercises"):
        updates.extend(str(p.relative_to(package)) for p in (package / folder).rglob("*.md"))
    updates = sorted(set(updates))
    for rel in updates:
        if not (package / rel).is_file():
            raise ValueError(f"Update package is incomplete: {rel}")
    # Preflight every target before backing up or writing. Do not follow links
    # that could put a copy/move outside the intended repository.
    for rel in (*updates, *OBSOLETE):
        target = repo / rel
        if repo not in target.resolve().parents or target.is_symlink():
            raise ValueError(f"Unsafe linked target: {rel}")
        if target.exists() and not target.is_file():
            raise ValueError(f"Expected a file at {rel}")
    backup = repo.parent / (repo.name + "-before-v2.1-" +
                            datetime.now().strftime("%Y%m%d-%H%M%S") + "-" + uuid.uuid4().hex[:6])
    backup.mkdir()
    print(f"Backup: {backup}", flush=True)
    # Back up ALL affected existing files before making the first update.
    for rel in sorted(set(updates) | set(OBSOLETE)):
        target = repo / rel
        if target.is_file():
            saved = backup / rel
            saved.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(target, saved)
    for rel in updates:
        target = repo / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(package / rel, target)
    for rel in OBSOLETE:
        target = repo / rel
        if target.is_file():
            target.unlink()  # Exact obsolete document; already backed up above.
            print(f"Removed obsolete document: {rel}")
    print("Updated member docs and checker. Student/reference C code, dependencies and builds were not touched.")
    return backup


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", required=True, type=Path)
    args = parser.parse_args()
    install(Path(__file__).resolve().parents[1], args.repo)


if __name__ == "__main__":
    main()
