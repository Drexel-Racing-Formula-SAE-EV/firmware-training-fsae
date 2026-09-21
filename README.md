# FSAE Firmware Training

STM32F407VG training for new Formula SAE firmware members. Projects 00–06 are
bare metal; Projects 07–10 introduce FreeRTOS, concurrency, fault management,
and a small ECU state machine. Zephyr is not used.

## Prerequisites

- Git and Python 3
- CMake and Ninja
- Arm GNU Toolchain (`arm-none-eabi-gcc`, `arm-none-eabi-gdb`)
- Renode

Initialize ST's source tree after cloning a Git repository with a committed
STM32CubeF4 submodule entry:

```powershell
.\tools\setup_dependencies.ps1
```

The trailing `m` in `--recursivem` is invalid; the command above is the exact
spelling.

## Build, run, and debug

From the repository root:

```powershell
python tools/build.py 00
python tools/run.py 00
python tools/test.py 10
```

Equivalent PowerShell entry points:

```powershell
.\tools\build.ps1 -Project 00
.\tools\run.ps1 -Project 00
.\tools\debug.ps1 -Project 00
```

Build artifacts are placed in `build/<project>/`, for example
`build/00/project00.elf`.

## Project sequence

| Project | Subject | Primary lesson |
|---|---|---|
| 00 | Bring-up | reset-to-main, ELF, UART, GDB |
| 01 | GPIO polling | digital input/output and polling cost |
| 02 | GPIO interrupts | EXTI, NVIC, ISR-to-main event handoff |
| 03 | Cooperative scheduler | SysTick, periodic jobs, wraparound-safe time |
| 04 | UART CLI | RX interrupt, ring buffer, command parser |
| 05 | Sensor monitor | ADC conversion, validity, staleness, hysteresis |
| 06 | Bare-metal CAN | frames, filters, rolling counters, timeouts |
| 07 | FreeRTOS fundamentals | periodic tasks and stack monitoring |
| 08 | RTOS synchronization | queues, mutexes, ISR semaphores |
| 09 | Fault manager | health supervision, latching, safe state |
| 10 | Mini ECU | INIT/READY/ACTIVE/FAULT integration |

Each project is independently buildable. Select it with `TRAINING_PROJECT`;
only that project's `main()` is linked.

## Renode controls

Projects 01 and 02 use the Discovery model's `UserButton` and `UserLED`:

```text
sysbus.gpioPortA.UserButton Press
sysbus.gpioPortA.UserButton Release
```

Projects 04 and 05 accept input in the UART4 analyzer window.
Projects 06–10 launch a second virtual STM32 node on a shared CAN hub; its
UART window shows the generated command traffic.

Renode's current STM32F4 model does not implement ADC1. Project 05 therefore
uses an injected raw-count backend in simulation while retaining an actual HAL
ADC1/PA1 backend for hardware builds (`-DTRAINING_RENODE=OFF`). Application and
sensor-monitoring logic is identical in both cases.

## Validation and dependency setup

Read `docs/VALIDATION.md` for test evidence and outstanding runtime checks.
`tools/test.py` runs native regressions and structure checks, not the MCU.
It requires a native `gcc` or `clang` in addition to the Arm compiler.

Hardware build: `python tools/build.py 05 --hardware --clean`. Hardware mode
accepts only `help` and `status`. The run/debug helpers target Renode.

The build helper verifies ARM ELF vectors and strong interrupt symbols and
records Cube/nested submodule revisions and compiler version in
`build/<project>/dependencies.txt`.

A ZIP contains neither the dependency source trees nor Git submodule gitlinks. In a new
Git repository without an existing submodule, add it once using
`git submodule add https://github.com/STMicroelectronics/STM32CubeF4.git third_party/STM32CubeF4`,
then run `tools/setup_dependencies.ps1`. Do not repeat the Cube add command over
an existing checkout. FreeRTOS-Kernel is checked out exactly at `V11.3.0`.

## Scope

No RTOS code is present before Project 07. Projects 11–13 remain optional future
work described in [`docs/ROADMAP.md`](docs/ROADMAP.md).
