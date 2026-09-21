param(
    [ValidateSet("00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10")]
    [string]$Project = "00",
    [switch]$Student
)
$ErrorActionPreference = "Stop"
& "$PSScriptRoot\build.ps1" -Project $Project -Student:$Student
$Arguments = @("$PSScriptRoot\run.py", $Project)
if ($Student) { $Arguments += "--student" }
python @Arguments
if ($LASTEXITCODE -ne 0) { throw "Renode failed ($LASTEXITCODE)." }
