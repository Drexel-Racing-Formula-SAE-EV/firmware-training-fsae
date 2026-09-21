# Windows setup

Use PowerShell from the repository folder. Required tools: Git, Python 3, CMake, Ninja, Arm GNU compiler/GDB, and Renode. Native gcc or clang is also needed for host behavior checks.

## 1. Check installed tools

```powershell
cd C:\\FSAE\\firmware-training
git --version
python --version
cmake --version
ninja --version
arm-none-eabi-gcc --version
arm-none-eabi-gdb --version
renode --version
Get-Command gcc, clang -ErrorAction SilentlyContinue
```

The final command needs to find at least one native compiler for host tests. You can build and simulate ARM firmware without it, but host checks will report BLOCKED.

## 2. Download dependencies

For a Git clone with registered submodules:

```powershell
.\\tools\\setup_dependencies.ps1
```

A downloaded ZIP does not contain dependencies or Git submodule registrations. For a fresh ZIP installation only, initialize Git if needed and add Cube once:

```powershell
git init
git submodule add https://github.com/STMicroelectronics/STM32CubeF4.git third_party/STM32CubeF4
.\\tools\\setup_dependencies.ps1
```

Do not repeat the Cube add command over an existing checkout. The setup helper initializes nested dependencies and selects FreeRTOS-Kernel V11.3.0.

## 3. Confirm the toolchain with the reference

```powershell
python tools/build.py 00
python tools/run.py 00
```

The UART4 window should show both banners and increasing heartbeat values. Close Renode afterwards. This is a reference demonstration, not your assignment.

## 4. Begin coding

Open [Lab 00](../exercises/00_bringup/README.md), edit its source, and use:

```powershell
python tools/build.py 00 --student
python tools/exercise.py check 00
python tools/run.py 00 --student
```

Before implementation, the starter prints its student banner and the checker reports NOT YET IMPLEMENTED. Follow the lab requirements to add the missing behavior.

[Back to lab index](../README.md) · [Troubleshooting](troubleshooting.md)
