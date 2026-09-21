param(
    [ValidateSet("00", "01", "02", "03", "04", "05")]
    [string]$Project = "00"
)
$ErrorActionPreference = "Stop"
& "$PSScriptRoot\build.ps1" -Project $Project
python "$PSScriptRoot\run.py" $Project

