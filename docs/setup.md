# Windows setup and first run

Open PowerShell in `C:\FSAE\firmware-training`, then verify:

```powershell
git --version
python --version
cmake --version
ninja --version
arm-none-eabi-gcc --version
arm-none-eabi-gdb --version
renode --version
```

Initialize dependencies and build Project 00:

```powershell
git submodule update --init --recursive
python tools/build.py 00
python tools/run.py 00
```

Expected UART output:

```text
FSAE Firmware Training
Project 00 - Bringup
heartbeat 0
heartbeat 1
```

For debugging:

```powershell
.\tools\debug.ps1 -Project 00
```

The debug Renode script loads the ELF and opens port 3333 without starting the
CPU. GDB connects, sets `break main`, and continues from reset.

If CMake reports that STM32CubeF4 is missing, check:

```powershell
git submodule status
Test-Path .\third_party\STM32CubeF4\Drivers\CMSIS
```

