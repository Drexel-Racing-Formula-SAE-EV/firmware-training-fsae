#!/usr/bin/env python3
"""Fast structural validation; does not pretend to replace an ARM build."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
required = [
    ROOT / "CMakeLists.txt",
    ROOT / "cmake/arm-none-eabi.cmake",
    ROOT / "platform/stm32f4/linker/STM32F407VGTx_FLASH.ld",
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
print(f"repository structure: PASS ({len(required)} required files)")

