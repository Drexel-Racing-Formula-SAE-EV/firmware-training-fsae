# Student labs v2.0 validation — 2026-09-21

Member-facing cleanup v2.1: default checks now report remaining TODOs before
running behavioral tests. Use --test-incomplete to reproduce the intentional
starter assertion failures below. Reference firmware and student C sources are unchanged.

Cleanup verification: four member-workflow/update-preservation tests passed,
as did reference host regressions, the existing 22-build source-selection audit,
baseline hashes and local Markdown link checks. The default Lab 00 check returns
incomplete without starting the assertion test. The reference Lab 00 test passes.
The update installer was exercised on disposable repositories on Linux;
Windows execution still needs confirmation. Backups are saved beside the target
repository before changes, and the installer never copies student C sources.

## Scope

11 independently selectable exercises, 19 student C sources and 25 numbered
implementation tasks. Each lab has requirements, source locations, exact commands,
manual acceptance, GDB tasks, signal mapping and review questions.
Original reference firmware sources are not moved or rewritten.

## Executed here

| Check | Result |
|---|---|
| ARM builds of untouched student 00–10 | 11/11 passed |
| ARM builds of reference 00–10 with updated build helpers | 11/11 passed |
| Build-time ELF/vector/debug/strong-handler checks | Passed for all built project and peer ELFs |
| Reference source baseline hashes (C/H/linker inputs) | 32/32 unchanged |
| Compiler source-selection audit | All 22 builds passed |
| CAN peer linker-map isolation | No student common library linked into peer |
| Student launcher generation, run + debug | 22/22 structural checks passed |
| Host behavior tests against reference sources | All 9 supported labs passed |
| Untouched student behavior tests | All 9 supported labs failed assertions as expected |
| Untouched student completion checks | All 11 reported incomplete |
| Existing reference tools/test.py 10 suite | Passed |
| Python helper syntax compilation | Passed |

The native behavior tests cover 00 banner/counter/delay intent, 01 GPIO HAL
configuration and IO, 02 EXTI setup intent/callback filtering, 03 time arithmetic,
04 RX buffering/recovery, 05 conversion/thresholds/recovery, 06 CAN validation
and signed big-endian encoding, 09 fault/latch boundaries, and 10 ECU transitions
and guarded recovery.

Labs 07/08 have no automatic FreeRTOS behavioral host harness. Their incomplete
status is detected by TODO checks; their behavior requires the manual procedures.
Lab 04's dispatcher is also manually evaluated, independent of its RX host tests.

## Not executed or established

- Interactive Renode acceptance for the new student workflow.
- Windows PowerShell execution of the updated wrappers (reviewed, not run here).
- Physical STM32 wiring, electrical checks, ADC calibration or CAN transceivers.
- Student implementations that do not yet exist.
- Full safety qualification, production readiness or complete fault coverage.

Host mocks are not simulated MCU interrupt/timing evidence. A compiler or host
test PASS must not be promoted to a Renode/hardware PASS.

## Reproduce

From repository root:

```powershell
python tools/verify_exercises.py
python tools/test.py 10
python tools/build.py 06 --student
python tools/exercise.py check 06
python tools/exercise.py check 06 --reference
```

The student check fails intentionally before implementing the lab.
After building every reference and student project, use
`python tools/verify_exercises.py --built` to audit actual compiler source paths
and peer linker maps. Follow every lab's README for the remaining runtime gates.

Toolchain used here: Arm GNU 14.3.1; execution host Linux. User Windows toolchain
versions can differ. Dependencies and toolchain provenance are recorded by the
build helper in each selected build directory.
