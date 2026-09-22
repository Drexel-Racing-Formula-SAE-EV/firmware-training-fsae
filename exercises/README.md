# Working through the labs

Use the [lab index](../README.md#choose-a-lab) and begin at 00.

## Your normal workflow

1. Read the lab README and locate its numbered TODOs.
2. Implement one task in `exercises/<lab>/src/`; keep the supplied function signatures.
3. Build: `python tools/build.py 00 --student`.
4. Check: `python tools/exercise.py check 00`.
5. Run: `python tools/run.py 00 --student`.
6. Complete the lab's Renode/GDB checklist and explain your implementation.

Replace 00 with your lab number. Build files are separate from reference output. `--clean` cleans only the selected build directory, not your work.

## Which window do I use?

| Window | What belongs here | Example |
|---|---|---|
| PowerShell | Build, check and launch commands | `python tools/build.py 00 --student` |
| Renode monitor | Control simulated machines and buttons | `sysbus.gpioPortA.UserButton Press` |
| Project UART4 | Your project's commands, where supported | `help` |
| CAN peer UART4 (06–10) | Send commands to your node over CAN | `enable on` |
| GDB prompt in PowerShell | Breakpoints and inspection | `next`, `info registers` |

The Project UART and CAN peer UART are different windows. Use each lab's exact command list; commands vary between projects. Enter one line at a time and press Enter. When a CAN peer is present, enter `mach set 0` in the Renode monitor before operating the project's button.

## What does my check result mean?

- **ELF build PASS:** your firmware compiled and basic ELF checks passed. Its behavior may still be incomplete.
- **NOT YET IMPLEMENTED:** numbered TODOs remain. Implement them, then remove their markers. This is the normal starter state.
- **BEHAVIOR CHECK FAILED:** the code did not meet a tested expectation. Read the failed check and revisit the relevant function.
- **Host checks passed:** proceed to the lab's manual simulation checklist. This does not certify hardware behavior.

To test partially completed work, use `python tools/exercise.py check 00 --test-incomplete`. Failed assertions can be expected for missing behavior. This still reports incomplete while TODO markers remain.

Host behavioral tests exist for 00–06, 09 and 10. Lab 04's dispatcher and the RTOS behavior in 07/08 require manual testing. All labs require their Renode acceptance procedure. Native gcc or clang is needed for host tests; Arm GCC alone is not enough.

## References and submission

The completed code remains in `projects/` and shared support folders. Attempt the task first, then compare your reasoning with the reference. Each lab is independent: later labs supply completed prerequisites rather than automatically importing your earlier work.

Submit your edited sources, available test results, the lab's required observations, and a brief explanation. Record any untested item as NOT RUN. Keep changes in Git so you can recover an earlier version.

For debugging, use `.\tools\debug.ps1 -Project 00 -Student`. The lab provides useful breakpoints. `tools/test.py` checks the reference suite; it is not the student grader.

[Signal maps](WIRING.md) · [Troubleshooting](../docs/troubleshooting.md) · [Instructor notes](../docs/maintainers/README.md)
