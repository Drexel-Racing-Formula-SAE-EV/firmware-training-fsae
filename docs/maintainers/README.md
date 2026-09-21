# Instructor notes

Current validation evidence is in [VALIDATION.md](VALIDATION.md).

## Source ownership

`manifest.json` maps every student source to its reference and enumerates the required TODOs. The mapping includes main.c even when the lab's work is in another module. The CMake selector replaces only selected files. For 06/09/10 it creates a separate student common library and links only the student project to it; the CAN peer keeps training_common.

Do not overwrite student work when distributing new packages. Commit or back up it first. Delivering this full repository exposes solutions: use your course access policy if students should not see them. This is a learning scaffold, not an anti-cheating system.

## Acceptance rules

Compilation, ELF validation, TODO completion, native logic tests, manual Renode behavior and physical hardware are separate gates. Record each separately. The lab README is the manual rubric. Do not award completion based on log text alone: inspect changed code and ask the learner to explain the invariants and diagnose one controlled failure.

Reference main and shared C sources remain unchanged. Student starters intentionally have incomplete behavior. 07/08 placeholder tasks block instead of spinning, 06 rejects all commands, and 09/10 placeholders request or remain in a safe/off state.

## Regression commands

```powershell
python tools/test.py 10
python tools/verify_exercises.py
python tools/exercise.py check 06 --reference
python tools/build.py 06 --student
python tools/exercise.py check 06
```

The last command should fail on an untouched starter. It should pass available host checks after a correct implementation, but manual runtime acceptance remains required. To inspect compiler source selection, open build/student/NN/compile_commands.json or use ninja -C build/student/NN -t commands. The build always verifies vectors and strong handlers, not business logic.

There is no regeneration command that could silently erase student work. A baseline SHA-256 manifest records the original reference C/H/linker/assembly inputs. The scaffold verifier checks baseline hashes, mappings and tests the Renode script transformation without starting Renode. Changes to reference code must deliberately update that baseline after review.
