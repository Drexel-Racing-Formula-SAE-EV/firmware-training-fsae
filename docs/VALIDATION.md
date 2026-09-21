# v0.8 review and validation — 2026-09-21

## Changes

- Link stm32f4xx_it.c directly into each executable so weak startup handlers
  cannot prevent archive extraction of application handlers.
- Set VTOR to flash before HAL_Init enables SysTick; check HAL_Init status.
- Verify ELF architecture, debug sections, stack and five handler vector entries.
- Mark BSS/stack reservation NOLOAD; separate read/execute and read/write segments.
- Track linker-script changes as link dependencies.
- Stop PowerShell run/debug after failed builds. Wait for the GDB port with
  a bounded readiness check and reject an already occupied port.
- UART TX has a 100 ms HAL timeout and error counters. RX error/rearm failures
  trigger recovery in main context. Overflow discards the damaged line through
  a subsequent newline; re-enter the command after the error. No partial command
  is intentionally executed. A functioning SysTick remains required for HAL timeouts.
- Project 02 counts serviced callbacks instead of collapsing them into a flag.
- Project 02 loads a local board overlay that preserves UserButton to GPIOA0
  and also routes UserButton directly to EXTI0. This works around the observed
  Renode 1.17 behavior where GPIOA.IDR changed but EXTI0 did not receive the
  transition. The firmware EXTI/NVIC configuration and handler remain unchanged.
- Project 03 runs each due job once, skips old releases while keeping phase,
  and reports missed-release counters. It does not replay catch-up bursts.
- Project 05 rejects empty/signed/nondecimal/out-of-range input; distinguishes
  invalid acquisition from no update; hardware builds hide injection commands.
- Build records actual dependency versions; no unverified dependency pin claimed.

## Evidence and limits

Native tests compile actual console, timebase, injected ADC and temperature
logic with mock HAL declarations for the platform tests. They cover line parsing,
CR/LF, backspace, overflow, partial lines, capacity changes, RX recovery, TX errors,
timing wraparound, skipped periods, acquisition states, threshold hysteresis,
staleness boundaries, recovery and numeric parsing. ELF verifier tests use
synthetic vectors, including deliberate weak handlers and corrupted entries.

The GNU archive/weak-symbol failure was reproduced on the host in review.
All six main.c files passed host C11 syntax checks against test HAL declarations;
Project 05 main.c also passed with the hardware branch selected. Linker syntax
and separate R/E and R/W load flags were checked using a host ELF fixture.
These are not cross-compilation results. The ELF checker permits ST's legitimate
weak Reset_Handler while requiring strong application interrupt handlers.
The host tests do not simulate actual Cortex-M interrupts or peripheral behavior.
Arm GNU, CMake/Ninja, Renode, PowerShell and STM32Cube sources are unavailable in
the packaging environment. Project 00 and Project 01 v0.7 runtime behavior was
confirmed from the user's Renode output. The v0.8 Project 02 overlay still
requires execution in the user's Renode installation.
The Windows dependency checkout is not accessible from here. The exact stalled
PC in the user's previous ELF remains unknown. Earlier claims that GPIO pull
configuration, polling delay or button press duration proved the cause were
unsupported and are withdrawn.

## Windows acceptance

Close existing Renode instances. Install all source files from this package,
preserving the existing Cube checkout. Run the host suite if native GCC/Clang
is installed: `python tools/test.py 05`.

Build each project with `python tools/build.py NN --clean`; require both the
link success and ELF verification PASS. CMake-only builds do not run the Python
ELF checker; invoke `python tools/verify_elf.py build/NN/projectNN.elf` explicitly.

| Project | Required runtime evidence |
|---|---|
| 00 | Both headings and at least five increasing heartbeats |
| 01 | Held press/release produces corresponding UART lines and PD12 states |
| 02 | Repeated press/release cycles toggle LED and print once per serviced event |
| 03 | Increasing ticks/counts and twice-per-second telemetry; inspect missed counts |
| 04 | All commands respond, uptime advances, overflow is rejected and next line works |
| 05 | 310 NORMAL, 745 WARNING, 931 FAULT; invalid and frozen inputs differ; recover |

Use `sysbus.gpioPortA.UserButton Press` and `Release` in the monitor, not UART.
For GDB require a connection to the new process and a stop at main; verify the
SysTick handler runs. Debug scripts now include the UART analyzer.
Project 05 is an injected sensor simulation, not validation of Renode ADC1.
Hardware ADC and Windows process launch behavior still require target testing.
