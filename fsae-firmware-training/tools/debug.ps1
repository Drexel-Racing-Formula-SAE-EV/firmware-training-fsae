param(
    [ValidateSet("00", "01", "02", "03", "04", "05")]
    [string]$Project = "00"
)
$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
& "$PSScriptRoot\build.ps1" -Project $Project

$Script = Join-Path $Repo "renode\project$Project-debug.resc"
$Elf = Join-Path $Repo "build\$Project\project$Project.elf"
Start-Process -FilePath "renode" -ArgumentList @($Script) -WorkingDirectory $Repo
Start-Sleep -Seconds 2
arm-none-eabi-gdb $Elf `
    -ex "target remote :3333" `
    -ex "break main" `
    -ex "continue"

