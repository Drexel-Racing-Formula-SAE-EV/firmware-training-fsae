# Windows setup: from downloads to your first lab

This guide targets an Intel/AMD x64 Windows 10/11 PC. Check **Settings → System → About → System type** first. Windows-on-ARM needs different host packages and is not covered by these instructions.

Already installed a tool? Run its verification command and skip reinstalling it if it works. Use **PowerShell** for repository commands; only the MSYS2 package commands below belong in its separate UCRT64 terminal. Do not copy a displayed `PS C:\...>` prompt into a command.

## Download checklist

**Follow this guide once.** If all tools are already installed, jump to [verify PATH](#9-fix-path-and-verify-everything), then [get the repository](#10-get-the-training-repository) and [verify your first build](#12-prove-setup-then-start-learning).

1. Install [Git](#1-git-for-windows), [Python](#2-python-3), [CMake](#3-cmake), [Ninja](#4-ninja), [Arm GNU](#5-arm-gnu-compiler-and-gdb), and [Renode](#6-renode).
2. Install [native GCC](#7-native-gcc-for-host-tests) for tests; [VS Code](#8-vs-code-optional) is optional.
3. [Check PATH](#9-fix-path-and-verify-everything), [get the code](#10-get-the-training-repository), and [download dependencies](#11-initialize-stm32cubef4-and-freertos).
4. [Run the reference once, then begin Lab 00](#12-prove-setup-then-start-learning).

| Tool | Purpose | Official download |
|---|---|---|
| Git for Windows | Download repository/dependencies and save changes | [Git](https://git-scm.com/install/windows) |
| Python 3 | Run the repository's helper scripts | [Python Windows releases](https://www.python.org/downloads/windows/) |
| CMake, at least 3.22 | Configure the firmware build | [CMake](https://cmake.org/download/) |
| Ninja | Execute the generated build | [Ninja releases](https://github.com/ninja-build/ninja/releases) |
| Arm GNU Toolchain | Compile/debug Cortex-M firmware | [Arm installation guide and downloads](https://learn.arm.com/install-guides/gcc/arm-gnu/) |
| Renode | Simulate the STM32 board and CAN peer | [Renode releases](https://github.com/renode/renode/releases) |
| MSYS2 UCRT64 GCC | Compile native Windows behavior tests | [MSYS2](https://www.msys2.org/) |
| VS Code (optional) | Edit code and read Markdown labs | [VS Code Windows setup](https://code.visualstudio.com/docs/setup/windows) |

GDB is included with the Arm toolchain; it is not a separate download.
PowerShell is already available on Windows. These labs do not require PuTTY,
STM32CubeIDE, Zephyr, Docker, WSL, or Robot Framework. The Python helpers use the
standard library; there is no project pip requirements installation.

Prefer stable releases over previews/nightlies unless the team supplies a
specific version. Download pages change: choose the package type described below,
not a source-code archive. The prior Windows reference session used Python 3.12,
Arm GNU 15.3.1 and Renode 1.17.0; that does not establish compatibility with every
newer release. Keep the versions that pass your first-build checks.

## 1. Git for Windows

1. Open the official Git download link above.
2. Download the **x64 Setup** installer and run it.
3. Enable command-line/third-party access to Git when the PATH option is offered.
4. Close and reopen PowerShell, then run:

```powershell
git --version
```

A version line confirms PowerShell can find Git. Installer names/versions are
listed on the [official Windows download page](https://git-scm.com/install/windows).

## 2. Python 3

1. Open [Python's Windows releases](https://www.python.org/downloads/windows/).
2. Choose a stable Python 3 release with a **Windows installer (64-bit)**.
   Do not choose the embeddable package or source tarball.
3. With the traditional installer, select **Add python.exe to PATH** before
   installing. Keep pip and the launcher enabled.
4. Reopen PowerShell and check:

```powershell
python --version
python -c "import sys; print(sys.executable)"
```

If you use Python's newer Install Manager instead, follow its prompts to install
a Python runtime and enable the python command. The [official Windows guide](https://docs.python.org/3/using/windows.html)
covers both runtime selection and command aliases.

If python opens Microsoft Store instead of showing a version, inspect
**Manage app execution aliases** in Windows Settings and the installed Python
PATH. Use the intended installed runtime; do not indiscriminately disable aliases
used by the Install Manager. The repository commands below require `python` to work.

## 3. CMake

1. Download the **Windows x64 Installer** from [CMake](https://cmake.org/download/).
2. Run the installer. If offered, add CMake to PATH for your user.
3. Otherwise add its installed bin folder through the PATH procedure below,
   commonly `C:\Program Files\CMake\bin`.
4. Reopen PowerShell:

```powershell
cmake --version
```

This repository requires CMake 3.22 or newer. You do not need the CMake GUI.

## 4. Ninja

1. Open [Ninja releases](https://github.com/ninja-build/ninja/releases).
2. Expand **Assets** for a stable release and download **ninja-win.zip** for x64.
   Do not choose Source code or the ARM64 package.
3. Extract ninja.exe to `C:\Tools\Ninja`.
4. Add that folder to your user PATH as described below, then reopen PowerShell:

```powershell
ninja --version
```

The directory in PATH must directly contain ninja.exe, not just its ZIP file.
CMake's Ninja generator expects that executable to be discoverable.

## 5. Arm GNU compiler and GDB

Open [Arm's installation guide](https://learn.arm.com/install-guides/gcc/arm-gnu/)
and follow its current download link. Choose:

- Host: **Windows**.
- Target: **arm-none-eabi** (bare-metal 32-bit Arm).
- Installer: **.msi** on newer releases, **.exe** on older ones.

A Windows host package may say mingw-w64-i686 even on a 64-bit PC. That is the
host build name; arm-none-eabi is the important firmware target. Do not choose
aarch64, arm-none-linux-gnueabihf, or a Linux/macOS archive.

Install it and enable its PATH option if available. Otherwise add the installed
**bin** directory that contains arm-none-eabi-gcc.exe. This is commonly underneath
`C:\Program Files (x86)\Arm\`; copy the actual folder from your PC
rather than guessing its version-dependent name.

Reopen PowerShell:

```powershell
arm-none-eabi-gcc --version
arm-none-eabi-gdb --version
arm-none-eabi-size --version
arm-none-eabi-objcopy --version
```

These four programs come from the same toolchain.

## 6. Renode

1. Open [Renode's stable releases](https://github.com/renode/renode/releases).
2. Select the Windows installer asset (.msi or .setup.exe depending on release),
   not the source archive or a Linux package.
3. Run the installer and enable its PATH option if offered. Otherwise add the
   installed directory containing the Renode launcher, commonly
   `C:\Program Files\Renode`.
4. Follow that release's prerequisite prompts. If it reports a missing .NET
   runtime, use the required version/architecture from the release instructions
   and [Microsoft's runtime downloads](https://dotnet.microsoft.com/en-us/download/dotnet).
   Do not assume an arbitrary newer runtime replaces the required major version.
5. Reopen PowerShell:

```powershell
renode --version
renode
```

Confirm the monitor opens, then close it. The project's run helper will open the
UART analyzer later. See [Renode's installation instructions](https://github.com/renode/renode/blob/master/README.md#installation)
for package formats and runtime dependencies.

## 7. Native GCC for host tests

**This is a second compiler.** Arm GCC builds the ELF for the simulated MCU.
Native GCC builds a Windows .exe so behavior tests can run directly on your PC.

1. Download the x86_64 installer from [MSYS2](https://www.msys2.org/).
2. Install to the default `C:\msys64`.
3. Open **MSYS2 UCRT64** from the Start menu.
4. In that UCRT64 terminal (not PowerShell), update packages:

```bash
pacman -Syu
```

If instructed to close the terminal, close it, reopen **MSYS2 UCRT64**, and run
the update again. Then install native GCC:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc
```

Accept the installation prompt. Add `C:\msys64\ucrt64\bin` to your
Windows user PATH. Do not add MSYS2's entire usr/bin directory for this workflow.

Open a new PowerShell window:

```powershell
gcc --version
gcc -dumpmachine
```

The target should identify Windows/mingw, not arm-none-eabi. Keep the UCRT64 bin
folder in PATH when running tests so compiler/runtime DLLs remain discoverable.
This package selection follows [MSYS2's GCC installation instructions](https://www.msys2.org/).

## 8. VS Code (optional)

Use the **Windows x64 User Installer** from the
[official VS Code setup page](https://code.visualstudio.com/docs/setup/windows).
Select its PATH option if offered, then reopen PowerShell. Open the repository
with `code .` or **File → Open Folder**.

The Microsoft C/C++ extension is optional for editing assistance. Neither an
extension nor VS Code installs the Arm compiler for you. Use the repository's
documented terminal commands for building and debugging.

## 9. Fix PATH and verify everything

PATH is a list of folders Windows searches when you type a command.

1. Open Start and search **Edit environment variables for your account**.
2. In **User variables**, select **Path → Edit → New**.
3. Add only the actual executable folder, one folder per entry. Do not replace
   the existing list and do not put quote characters around entries.
4. Confirm all dialogs. Close/reopen PowerShell and VS Code.

Typical entries to check (installation locations can differ):

| Tool | Folder to put in PATH |
|---|---|
| Git | C:\Program Files\Git\cmd |
| Python | The selected runtime folder; its Scripts folder if using pip commands |
| CMake | C:\Program Files\CMake\bin |
| Ninja | C:\Tools\Ninja |
| Arm GNU | The actual toolchain bin folder |
| Renode | The installed launcher folder |
| Native GCC | C:\msys64\ucrt64\bin |

In a **new PowerShell**, run:

```powershell
Get-Command git, python, cmake, ninja, arm-none-eabi-gcc, arm-none-eabi-gdb, arm-none-eabi-size, arm-none-eabi-objcopy, renode, gcc | Select-Object Name, Source
```

Every tool should resolve. For duplicates or an unexpected version:

```powershell
where.exe python
where.exe gcc
where.exe arm-none-eabi-gcc
```

Stop and fix missing commands before downloading dependencies or building.

## 10. Get the training repository

Choose one route. Do not clone or extract another repository inside an existing one.

**Git clone, for a new working folder:**

```powershell
New-Item -ItemType Directory -Force C:\FSAE | Out-Null
cd C:\FSAE
git clone --recurse-submodules https://github.com/Mahad-Faisal/firmware-training.git
cd firmware-training
```

Use the team's approved revision. These setup docs do not establish that a
downloaded package has already been pushed to GitHub. If cloning reports access
denied, ask the team for repository access or the approved package.

**Team ZIP, for a new working folder:**

Extract the approved ZIP to a temporary folder using File Explorer. Find the
folder containing CMakeLists.txt, exercises, tools and projects. Place that folder
at `C:\FSAE\firmware-training`. The intended file is
`C:\FSAE\firmware-training\CMakeLists.txt`, not a nested second copy.

Then open PowerShell there. ZIPs omit dependency source trees and Git submodule
registrations. For a fresh ZIP installation only:

```powershell
cd C:\FSAE\firmware-training
git init
git submodule add https://github.com/STMicroelectronics/STM32CubeF4.git third_party/STM32CubeF4
```

Do not run submodule add over an existing Cube checkout.

## 11. Initialize STM32CubeF4 and FreeRTOS

From the repository root:

```powershell
.\tools\setup_dependencies.ps1
```

This initializes nested Cube dependencies and selects FreeRTOS-Kernel V11.3.0.
Allow the Git downloads to finish. STM32 HAL/CMSIS and FreeRTOS come from these
repositories; no separate CubeIDE installation is needed.

If Windows blocks a downloaded script, review its contents first. On a machine
where you are permitted to run it, unblock that specific file:

```powershell
Unblock-File .\tools\setup_dependencies.ps1
```

If execution policy still blocks it, use the process-only setting below for this
PowerShell session, then rerun the helper. It does not change future sessions:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy RemoteSigned
.\tools\setup_dependencies.ps1
```

If an organization-enforced policy blocks execution, ask its administrator
instead of trying to override it. Other downloaded .ps1 wrappers may likewise
need individual review/unblocking; the Python build/run commands do not need
PowerShell script execution.

Verify dependency files:

```powershell
Test-Path .\third_party\STM32CubeF4\Drivers\CMSIS\Device\ST\STM32F4xx\Include\stm32f407xx.h
Test-Path .\third_party\STM32CubeF4\Drivers\STM32F4xx_HAL_Driver\Src\stm32f4xx_hal.c
Test-Path .\third_party\FreeRTOS-Kernel\tasks.c
git -C third_party/FreeRTOS-Kernel describe --tags --exact-match
```

Expect three True results and V11.3.0.

## 12. Prove setup, then start learning

First use the completed reference:

```powershell
python tools/build.py 00
python tools/exercise.py check 00 --reference
python tools/run.py 00
```

The build should produce build/00/project00.elf and pass its ELF checks.
The native check should pass. The UART4 analyzer should show:

```text
FSAE Firmware Training
Project 00 - Bringup
heartbeat 0
heartbeat 1
heartbeat 2
```

Observe several heartbeats, then close Renode. For a GDB check:

```powershell
.\tools\debug.ps1 -Project 00
```

GDB should stop at main in PowerShell. Enter list, next, or continue there.
Close the debugger/Renode session before opening another.

Now open [Lab 00](../exercises/00_bringup/README.md) and edit its TODOs:

```powershell
python tools/build.py 00 --student
python tools/exercise.py check 00
python tools/run.py 00 --student
```

The untouched student version intentionally lacks finished behavior. Its
NOT YET IMPLEMENTED result is expected. Implement it before expecting host checks
to pass. See [troubleshooting](troubleshooting.md) for warnings and which window
accepts each command.

[Back to lab index](../README.md)
