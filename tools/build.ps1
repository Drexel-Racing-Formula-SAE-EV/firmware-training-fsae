param(
    [ValidateSet("00", "01", "02", "03", "04", "05")]
    [string]$Project = "00",
    [switch]$Clean,
    [switch]$Hardware
)
$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Arguments = @("$PSScriptRoot\build.py", $Project)
if ($Clean) { $Arguments += "--clean" }
if ($Hardware) { $Arguments += "--hardware" }
python @Arguments
if ($LASTEXITCODE -ne 0) { throw "Firmware build failed ($LASTEXITCODE)." }
