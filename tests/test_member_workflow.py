"""Regression tests for member feedback and preservation during doc updates."""
import contextlib
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "tools"))
import exercise
import install_member_update


class MemberWorkflow(unittest.TestCase):
    def check(self, marker, extra, behavior):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            src = root / "exercises/lab/src/main.c"
            src.parent.mkdir(parents=True)
            src.write_text("/* TODO(00.1): implement */" if marker else "implemented")
            (root / "exercises/manifest.json").write_text(json.dumps({
                "projects": {"00": {"directory": "lab", "sources": {
                    "exercises/lab/src/main.c": "projects/lab/src/main.c"
                }}}
            }))
            out = io.StringIO()
            with patch.object(exercise, "ROOT", root), patch.object(
                    exercise, "run_host", return_value=behavior) as host, patch.object(
                    sys, "argv", ["exercise.py", "check", "00", *extra]), contextlib.redirect_stdout(out):
                result = exercise.main()
            return result, out.getvalue(), host.call_count

    def test_starter_does_not_launch_asserting_binary(self):
        code, output, calls = self.check(True, [], False)
        self.assertEqual(code, 1)
        self.assertEqual(calls, 0)
        self.assertIn("NOT YET IMPLEMENTED", output)
        self.assertIn("README.md", output)

    def test_partial_work_can_explicitly_run_tests(self):
        code, output, calls = self.check(True, ["--test-incomplete"], True)
        self.assertEqual(code, 1)
        self.assertEqual(calls, 1)

    def test_removed_markers_do_not_bypass_behavior(self):
        self.assertEqual(self.check(False, [], False)[0], 1)
        self.assertEqual(self.check(False, [], True)[0], 0)

    def test_update_preserves_student_code_and_backs_up_docs(self):
        with tempfile.TemporaryDirectory() as temp:
            base = Path(temp)
            package, repo = base / "package", base / "repo"
            for rel in ("README.md", "tools/exercise.py", "tools/run.py",
                        "tools/install_member_update.py", "tests/test_member_workflow.py",
                        "docs/setup.md", "exercises/lab/README.md"):
                path = package / rel
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text("updated")
            for rel, value in {
                "CMakeLists.txt": "build config", "exercises/manifest.json": "{}",
                "exercises/lab/src/main.c": "MY WORK", "README.md": "old readme",
                "docs/ROADMAP.md": "old roadmap", "docs/my-notes.md": "KEEP"
            }.items():
                path = repo / rel
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(value)
            with contextlib.redirect_stdout(io.StringIO()):
                backup = install_member_update.install(package, repo)
            self.assertEqual((repo / "exercises/lab/src/main.c").read_text(), "MY WORK")
            self.assertEqual((repo / "docs/my-notes.md").read_text(), "KEEP")
            self.assertEqual((repo / "README.md").read_text(), "updated")
            self.assertFalse((repo / "docs/ROADMAP.md").exists())
            self.assertEqual((backup / "docs/ROADMAP.md").read_text(), "old roadmap")
            self.assertEqual((backup / "README.md").read_text(), "old readme")


if __name__ == "__main__":
    unittest.main()
