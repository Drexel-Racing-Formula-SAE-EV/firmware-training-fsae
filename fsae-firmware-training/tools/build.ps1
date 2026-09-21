param(
    [ValidateSet("00", "01", "02", "03", "04", "05")]
    [string]$Project = "00",
    [switch]$Clean
)
$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
$Arguments = @("$PSScriptRoot\build.py", $Project)
if ($Clean) { $Arguments += "--clean" }
python @Arguments

