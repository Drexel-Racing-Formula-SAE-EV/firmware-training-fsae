#!/usr/bin/env python3
"""Fast structural validation; does not pretend to replace an ARM build."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
required = [
    ROOT / "CMakeLists.txt",
    ROOT / "cmake/arm-none-eabi.cmake",
    ROOT / "platform/stm32f4/linker/STM32F407VGTx_FLASH.ld",
    ROOT / "renode/project02-button-exti.repl",
]
for number, name in [
    ("00", "00_bringup"), ("01", "01_gpio_polling"),
    ("02", "02_gpio_interrupt"), ("03", "03_timer_scheduler"),
    ("04", "04_uart_cli"), ("05", "05_adc_sensor")]:
    required.extend([
        ROOT / "projects" / name / "CMakeLists.txt",
        ROOT / "projects" / name / "README.md",
        ROOT / "renode" / f"project{number}.resc",
    ])

missing = [str(path.relative_to(ROOT)) for path in required if not path.is_file()]
if missing:
    raise SystemExit("Missing required files:\n" + "\n".join(missing))

cmake = (ROOT / "platform/stm32f4/CMakeLists.txt").read_text(encoding="utf-8")
linker = (ROOT / "platform/stm32f4/linker/STM32F407VGTx_FLASH.ld").read_text(
    encoding="utf-8"
)

problems = []
library_sources = cmake.split('function(add_training_firmware')[0]
firmware_sources = cmake.split('function(add_training_firmware')[-1]
if 'src/stm32f4xx_it.c' in library_sources or 'src/stm32f4xx_it.c' not in firmware_sources:
    problems.append('IRQ handlers must be executable sources, not archive-only sources')
if 'LINK_DEPENDS' not in cmake:
    problems.append('linker script must trigger relinking')
if '-T"' in cmake or "-T'" in cmake:
    problems.append("linker-script option contains literal quotes")
if 'SUFFIX ".elf"' not in cmake:
    problems.append("firmware target does not explicitly use the .elf suffix")
if "RAM   (xrw) : ORIGIN = 0x20000000, LENGTH = 128K" not in linker:
    problems.append("main SRAM region is not the STM32F407's contiguous 128 KiB")
if "CCMRAM (rw) : ORIGIN = 0x10000000, LENGTH = 64K" not in linker:
    problems.append("64 KiB CCM region is missing or incorrect")

for number in range(6):
    project = f"{number:02d}"
    resc = (ROOT / "renode" / f"project{project}.resc").read_text(encoding="utf-8")
    expected = f"@build/{project}/project{project}.elf"
    if expected not in resc:
        problems.append(f"project{project}.resc does not load {expected}")

project02_resc = (ROOT / "renode/project02.resc").read_text(encoding="utf-8")
project02_debug = (ROOT / "renode/project02-debug.resc").read_text(encoding="utf-8")
project02_overlay = (ROOT / "renode/project02-button-exti.repl").read_text(
    encoding="utf-8"
)
overlay_reference = "@renode/project02-button-exti.repl"
if overlay_reference not in project02_resc or overlay_reference not in project02_debug:
    problems.append("Project 02 run and debug scripts must load the EXTI button overlay")
if "-> exti@0" not in project02_overlay:
    problems.append("Project 02 overlay must connect UserButton directly to EXTI0")
if "-> gpioPortA@0" in project02_overlay:
    problems.append("Project 02 overlay cannot fan out one Button IRQ to two destinations")

if problems:
    raise SystemExit("Repository validation failed:\n" + "\n".join(problems))
print(f"repository structure: PASS ({len(required)} required files)")
