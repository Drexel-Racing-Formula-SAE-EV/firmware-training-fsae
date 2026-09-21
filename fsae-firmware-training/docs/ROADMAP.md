# Curriculum roadmap after Project 05

Projects 06–10 are release gates. Advanced Projects 11–13 are optional and
must not delay the core curriculum.

## Project 06 — Bare-metal CAN node

- Configure CAN1 and its RX FIFO interrupt before introducing an RTOS.
- Transmit heartbeat `0x100` and temperature `0x101` frames.
- Receive command `0x200`; validate identifier, frame type, and DLC before use.
- Detect a missing periodic command with wraparound-safe timeout arithmetic.
- Test TX, RX, filter rejection, bad DLC, rolling counter, and timeout in
  Renode. This is the bare-metal graduation project.

Gate: virtual CAN must work reliably before later projects depend on it.

## Project 07 — FreeRTOS fundamentals

- Pin an official FreeRTOS-Kernel release as a dependency.
- Compare Project 06's cooperative loop with task-based structure.
- Create sensor, control, and telemetry tasks using `vTaskDelayUntil`.
- Measure task periods and stack high-water marks.
- Re-run UART, GPIO interrupt, and CAN smoke tests after scheduler start.

Gate: scheduler, several tasks, periodic delays, and interrupts all work.

## Project 08 — queues, mutexes, semaphores

- Move typed samples through a queue.
- Protect shared UART output with a mutex.
- Wake a processing task from an ISR with a binary semaphore and the correct
  `FromISR` API/yield pattern.
- Demonstrate and diagnose queue-full, priority, and deadlock mistakes.

## Project 09 — health and fault manager

- Supervise task heartbeats and communication deadlines.
- Centralize warnings, active faults, latched faults, and safe-state requests.
- Inject sensor, communication, and task-stall faults deterministically.
- Keep diagnostic communication alive in the fault state.

## Project 10 — mini ECU capstone

- States: INIT, READY, ACTIVE, FAULT.
- Inputs: sensor validity, enable command, communication freshness, task health.
- Outputs: LED/actuator abstraction, CAN heartbeat, CLI status/fault report.
- Qualification scenario: boot → READY → enable → ACTIVE → injected sensor
  fault → FAULT → report → controlled recovery where permitted.

## Advanced 11 — two virtual ECUs

Connect VCU and AMS machines over virtual CAN. Test normal enable/heartbeat
traffic and failures caused by stale frames, invalid counters, and one ECU
being paused or reset.

## Advanced 12 — SPI sensor driver

Implement a small register-oriented driver, validate chip ID and status bits,
handle transaction errors/timeouts, and separate bus transport from sensor
conversion logic.

## Advanced 13 — watchdog

Configure the independent watchdog, service it only after all required health
checks pass, deliberately stall a required execution path, and prove reset
cause reporting after reboot.

## Repository expansion rule

Add one project at a time. Each addition needs a build, a real Renode execution
test for modeled hardware, host tests for pure logic, acceptance criteria, and
a code-review checklist. Do not report a peripheral complete because it only
compiled.

