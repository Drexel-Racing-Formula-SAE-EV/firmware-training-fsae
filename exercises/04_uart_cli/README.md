# Lab 04: uart cli

Implement bounded ISR RX publication and an exact-match UART command dispatcher.

## Starting point

UART initialization, RX rearming/error handling scaffold, consumer line assembly and recovery are supplied.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `04.1` — `src/main.c`, `run_command`: Implement exact help/status/uptime/gpio/timer/version commands and an unknown-command response.
- `04.2` — `src/console.c`, `HAL_UART_RxCpltCallback`: Publish rx_byte into the bounded SPSC ring before advancing rx_head; detect full, count overflow, mark corrupt, and keep RX rearming.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 04 --student
python tools/exercise.py check 04
python tools/run.py 04 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/04/project04.elf`. The reference remains `python tools/build.py 04` / `python tools/run.py 04`. Close an earlier Renode session before launching another.

Native checks exercise the student RX driver; command dispatch is manual.

## Acceptance procedure

1. Complete ring publication in HAL_UART_RxCpltCallback; publish byte before head, reserve a slot to distinguish full from empty, and count/mark overflow.
2. Implement only help, status, uptime, gpio, timer, version and an unknown-command error in run_command.
3. Run host checks for RX, CRLF, partial lines, backspace, overflow and UART-error recovery. The dispatcher also requires the manual checks below.
4. In the UART analyzer enter each supported command separately. gpio toggles the LED; uptime increases; bogus is rejected; an overlong line is rejected and the next help succeeds.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 04 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break HAL_UART_RxCpltCallback
break run_command
print rx_head
print rx_tail
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- Which context owns each ring index?
- Why must corrupted input be discarded through the next line terminator?
- Why is a prefix comparison inappropriate for the exact command gpio?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
