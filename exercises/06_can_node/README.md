# Lab 06: can node

Implement CAN command validation, freshness and binary telemetry encoding.

## Starting point

bxCAN initialization/filter/IRQ queue and both main loops are supplied. The peer always uses reference protocol code.
Only edit the files listed below. Earlier concepts are supplied independently in each lab; completing one lab does not automatically copy its code into later labs.

## Coding tasks

- `06.1` — `src/training_can_protocol.c`, `training_command_process`: Validate ID, type, DLC, enable value and rolling counter before updating state; count each rejection and accepted command.
- `06.2` — `src/training_can_protocol.c`, `training_command_fresh`: Require valid and unsigned age <= TRAINING_COMMAND_TIMEOUT_MS; support millisecond wrap.
- `06.3` — `src/training_can_protocol.c`, `training_can_make_temperature`: Encode signed millidegrees in four big-endian bytes after initializing standard data frame.

Keep function signatures and public headers unchanged. Replace the placeholder and remove its TODO marker only after implementing the requirement. Do not silence tests or copy the solution merely to obtain PASS.

## Build and check

From repository root in PowerShell:

```powershell
python tools/build.py 06 --student
python tools/exercise.py check 06
python tools/run.py 06 --student
```

Untouched starters compile, but checks intentionally report INCOMPLETE (and behavioral tests may fail). The student ELF is `build/student/06/project06.elf`. The reference remains `python tools/build.py 06` / `python tools/run.py 06`. Close an earlier Renode session before launching another.

Native protocol tests compile student protocol; peer does not use it.

## Acceptance procedure

1. Validate standard data frames, ID 0x200, DLC 2, enable byte 0/1, then expected rolling counter while prior command is fresh.
2. On acceptance update valid/enable/counter/timestamp/accepted together. On rejection increment the correct diagnostic without refreshing the accepted command timestamp.
3. Require age <=300 ms and handle counter 255 ->0 and millisecond wrap; permit counter resynchronization after timeout.
4. In the peer UART use enable on, enable off, bad id, bad dlc, bad counter. Inspect node rejection counts. pause for two seconds gives TIMEOUT; resume recovers. The node has no command CLI.

Type each UART command on its own line and press Enter. Keep separate windows for Project and CAN peer. Renode monitor commands belong in the monitor, not a UART. Allow periodic tasks at least one second after changing a condition unless the procedure says otherwise.

## GDB exercise

Run `.\\tools\\debug.ps1 -Project 06 -Student`. At the initial main breakpoint, useful commands (enter separately) are:

```text
break training_command_process
print *frame
print *command
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## Explain before signing off

- Why does rejected traffic not refresh the deadline?
- Why does a fresh command require the next counter, but stale reception permits resynchronization?
- What is the big-endian byte representation of -1000 mC?

## Evidence to submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.
