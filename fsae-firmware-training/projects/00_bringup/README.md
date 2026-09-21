# Project 00 — Firmware bring-up

## Objective

Complete the build → ELF → Renode → UART → GDB loop.

## Concepts and background

Reset loads the initial stack pointer and reset vector. Startup code initializes
`.data` and `.bss`, calls `SystemInit`, and enters `main`. The linker script maps
the vector table/code into flash and runtime data/stack into SRAM.

## Provided code

Clock setup, UART4 console, STM32 startup source from STM32CubeF4, and the
STM32F407VG linker script.

## Student tasks

Build and run; locate the ELF/map/HEX/BIN; change the heartbeat text and period;
use GDB to break at `main`, step, inspect `counter`, commit, and open a PR.

## Requirements and acceptance criteria

UART prints both headings and a monotonic heartbeat. Explain reset, vector
table, reset handler, stack, `.data`, `.bss`, system initialization, and `main`.

## Expected output

```text
FSAE Firmware Training
Project 00 - Bringup
heartbeat 0
heartbeat 1
```

## Build, run, and tests

```powershell
python tools/build.py 00
python tools/run.py 00
.\tools\debug.ps1 -Project 00
```

## Common mistakes, review questions, and stretch goals

Wrong Arm target, missing submodule, stale build directory, or loading the wrong
ELF. Review: what happens before `main`? Stretch: inspect symbols and sections
with `arm-none-eabi-nm` and `arm-none-eabi-objdump -h`.

