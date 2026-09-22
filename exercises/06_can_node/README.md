# Lab 06: can node

Implement CAN command validation, freshness and binary telemetry encoding.

## 1. What is supplied

bxCAN initialization/filter/IRQ queue and both main loops are supplied. The peer always uses reference protocol code.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `06.1` — `src/training_can_protocol.c`, `training_command_process`: Validate ID, type, DLC, enable value and rolling counter before updating state; count each rejection and accepted command.
- `06.2` — `src/training_can_protocol.c`, `training_command_fresh`: Require valid and unsigned age <= TRAINING_COMMAND_TIMEOUT_MS; support millisecond wrap.
- `06.3` — `src/training_can_protocol.c`, `training_can_make_temperature`: Encode signed millidegrees in four big-endian bytes after initializing standard data frame.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 06 --student
python tools/exercise.py check 06
python tools/run.py 06 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/06/project06.elf`. The reference remains `python tools/build.py 06` / `python tools/run.py 06`. Close an earlier Renode session before launching another.

Native protocol tests compile student protocol; peer does not use it.

## 4. Test your implementation

1. Validate standard data frames, ID 0x200, DLC 2, enable byte 0/1, then expected rolling counter while prior command is fresh.
2. On acceptance update valid/enable/counter/timestamp/accepted together. On rejection increment the correct diagnostic without refreshing the accepted command timestamp.
3. Require age <=300 ms and handle counter 255 ->0 and millisecond wrap; permit counter resynchronization after timeout.
4. In the peer UART use enable on, enable off, bad id, bad dlc, bad counter. Inspect node rejection counts. pause for two seconds gives TIMEOUT; resume recovers. The node has no command CLI.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 06 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
break training_command_process
continue
print *frame
print *command
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- Why does rejected traffic not refresh the deadline?
- Why does a fresh command require the next counter, but stale reception permits resynchronization?
- What is the big-endian byte representation of -1000 mC?

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
