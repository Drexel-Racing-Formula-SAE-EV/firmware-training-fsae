param(
    [ValidateSet("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10")]
    [string]$Project = "00"
)
$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
& "$PSScriptRoot\build.ps1" -Project $Project

$Script = Join-Path $Repo "renode\project$Project-debug.resc"
$Elf = Join-Path $Repo "build\$Project\project$Project.elf"
function Test-GdbPort {
    # Do not probe the GDB server by connecting a TcpClient. Renode treats that
    # probe as a debugger connection and can start the CPU before the real GDB
    # client installs its breakpoint at main(). Inspect the local listener list
    # instead, which has no effect on the emulated target.
    $Listeners = [System.Net.NetworkInformation.IPGlobalProperties]::GetIPGlobalProperties().GetActiveTcpListeners()
    return [bool]($Listeners | Where-Object { $_.AddressFamily -eq [System.Net.Sockets.AddressFamily]::InterNetwork -and $_.Port -eq 3333 })
}
if (Test-GdbPort) { throw 'Port 3333 is occupied. Close the previous debugger/Renode session.' }
$RenodeProcess = Start-Process -FilePath 'renode' -ArgumentList ('"' + $Script + '"') -WorkingDirectory $Repo -PassThru
$Deadline = (Get-Date).AddSeconds(30)
while (-not (Test-GdbPort)) {
    if ($RenodeProcess.HasExited) { throw 'Renode exited before its GDB server became ready.' }
    if ((Get-Date) -gt $Deadline) { throw 'Timed out waiting for Renode GDB port 3333. Inspect its monitor.' }
    Start-Sleep -Milliseconds 200
}
arm-none-eabi-gdb $Elf `
    -ex "target remote :3333" `
    -ex "break main" `
    -ex "continue"
if ($LASTEXITCODE -ne 0) { throw "GDB failed ($LASTEXITCODE)." }
