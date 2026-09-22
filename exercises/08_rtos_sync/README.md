# Lab 08: rtos sync

Implement queue producer/consumer work and ISR-safe semaphore signaling.

## 1. What is supplied

Queue (four messages), mutex/semaphore creation, task creation, CAN, CLI and flood harness are supplied.
Edit the files listed below, inside this lab folder. Each lab supplies its own prerequisites; your earlier solutions are not copied into it.

## 2. What you write

- `08.1` — `src/main.c`, `HAL_GPIO_EXTI_Callback`: For PA0 and an existing semaphore, increment events, give with FromISR, and request a yield if needed.
- `08.2` — `src/main.c`, `sensor_task`: Every 100 ms enqueue sequence/time/raw by value with zero wait; count failed sends via record_queue_drop.
- `08.3` — `src/main.c`, `processing_task`: Honor flood_active using task notification, block on queue receive, and publish latest_sample inside a short critical section.

Keep the supplied function signatures and headers. Implement each TODO before removing its marker.

## 3. Build and run

From repository root in PowerShell:

```powershell
python tools/build.py 08 --student
python tools/exercise.py check 08
python tools/run.py 08 --student
```

An untouched starter should compile. Its check reports **NOT YET IMPLEMENTED** until you complete the TODOs. The student ELF is `build/student/08/project08.elf`. The reference remains `python tools/build.py 08` / `python tools/run.py 08`. Close an earlier Renode session before launching another.

Manual FreeRTOS synchronization/overflow checks required.

## 4. Test your implementation

1. Producer builds a value message with sequence/time/raw; send without blocking and increment drops through record_queue_drop on failure.
2. Consumer blocks on queue receive and publishes a coherent latest_sample. Preserve the flood_active notification handshake described in the TODO.
3. ISR gives the button semaphore with the FromISR API and performs the conditional ISR yield; button task does work/logging.
4. UART commands: help, status, queue flood. Ordinary operation advances latest_seq; flood raises drops, then normal messages continue. Drops are not required to equal a fixed number.
5. Use mach set 0 and Press/Release to wake button task. Peer pause/resume tests FRESH/TIMEOUT. Check logs are not interleaved within a message.

Enter one command at a time and press Enter. Use the [command-window guide](../README.md#which-window-do-i-use) if unsure where to type. Wait at least one second for periodic output unless a step specifies otherwise.

## 5. Inspect with GDB

Run `.\tools\debug.ps1 -Project 08 -Student`. The debugger first stops at `main`. Enter the commands below one at a time. After `continue`, wait for the selected breakpoint; trigger the relevant input if needed.

```text
break processing_task
continue
print queue_drops
print latest_sample
print button_events
```

Continue to the relevant code before inspecting locals. Ctrl+C interrupts a running target. A breakpoint pauses execution and changes timing, so assess deadlines without debugger stops too.

## 6. Explain your work

- Why send the struct by value rather than a pointer to a stack variable?
- Why does a binary semaphore coalesce multiple edges?
- Why are mutexes inappropriate in an ISR?

## 7. Submit

- Completed student source and a short explanation of each TODO.
- Build log, available host-test results, and annotated UART/GDB observations for every acceptance step.
- One deliberately introduced bug, its symptom, and how you located it.
- Record NOT RUN for any unavailable hardware or runtime check; never infer a pass from compilation.

Reference code locations are listed in [the manifest](../manifest.json); compare only after attempting the lab. See [wiring](wiring.md) for scope and signals.

[All labs](../../README.md#choose-a-lab) · [Workflow and check results](../README.md)
