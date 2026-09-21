# FSAE Firmware Training

Learn STM32 firmware by completing 11 coding labs in Renode. No physical board is needed to start. Projects 00–06 are bare metal; 07–10 use FreeRTOS.

## Start here

1. Follow [Windows setup](docs/setup.md) once.
2. Open [Lab 00](exercises/00_bringup/README.md).
3. Edit the numbered TODOs in that lab's `exercises/.../src/` files.
4. Build, check and run your code from the repository root:

```powershell
python tools/build.py 00 --student
python tools/exercise.py check 00
python tools/run.py 00 --student
```

An untouched lab builds but is intentionally incomplete. `NOT YET IMPLEMENTED` tells you what to work on; it is not an installation problem. Finish the TODOs before expecting the lab's behavior tests to pass.

## Choose a lab

| Lab | What you implement |
|---|---|
| [00](exercises/00_bringup/README.md) | Banners and heartbeat; trace startup with GDB |
| [01](exercises/01_gpio_polling/README.md) | GPIO configuration and button/LED IO |
| [02](exercises/02_gpio_interrupt/README.md) | EXTI setup and interrupt event handoff |
| [03](exercises/03_timer_scheduler/README.md) | Wraparound-safe cooperative scheduling |
| [04](exercises/04_uart_cli/README.md) | UART receive buffering and commands |
| [05](exercises/05_adc_sensor/README.md) | Sensor conversion, validity and hysteresis |
| [06](exercises/06_can_node/README.md) | CAN validation, counters and timeouts |
| [07](exercises/07_freertos_tasks/README.md) | Periodic FreeRTOS task bodies |
| [08](exercises/08_rtos_sync/README.md) | Queues and ISR semaphore signaling |
| [09](exercises/09_fault_manager/README.md) | Fault detection, latching and clearing |
| [10](exercises/10_mini_ecu/README.md) | ECU state transitions and guarded recovery |

Each lab tells you which files to edit, how to test them, and what evidence to submit.

## Where things belong

| Location | Purpose |
|---|---|
| `exercises/` | Your editable labs and instructions |
| `projects/` | Completed reference applications; compare after your attempt |
| `platform/`, `common/` | Shared support/reference modules; edit a lab's copy when instructed |
| `tools/`, `tests/`, `renode/` | Build, checks, simulation and debugging support |
| `third_party/` | Downloaded dependencies; do not edit |
| `build/student/NN/` | Generated student output; do not edit |

Use `--student` for your work. Omitting it builds/runs the completed reference in `build/NN/`. To debug your work:

```powershell
.\\tools\\debug.ps1 -Project 00 -Student
```

Need help? [Troubleshooting](docs/troubleshooting.md) covers test messages, UART windows, GDB and common build issues. [Lab workflow](exercises/README.md) explains completion requirements. Optional physical signal maps are in each lab's wiring notes.
