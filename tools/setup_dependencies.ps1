$ErrorActionPreference = "Stop"
$Repo = Split-Path -Parent $PSScriptRoot
Set-Location $Repo

git submodule update --init --recursive
if ($LASTEXITCODE -ne 0) { throw "STM32CubeF4 submodule initialization failed." }

$FreeRtos = Join-Path $Repo "third_party\FreeRTOS-Kernel"
if (-not (Test-Path (Join-Path $FreeRtos "tasks.c"))) {
    git submodule add --force `
        https://github.com/FreeRTOS/FreeRTOS-Kernel.git `
        third_party/FreeRTOS-Kernel
    if ($LASTEXITCODE -ne 0) { throw "Adding FreeRTOS-Kernel failed." }
}
git -C $FreeRtos fetch --tags
if ($LASTEXITCODE -ne 0) { throw "Fetching FreeRTOS tags failed." }
git -C $FreeRtos checkout V11.3.0
if ($LASTEXITCODE -ne 0) { throw "Checking out FreeRTOS V11.3.0 failed." }

$Tag = git -C $FreeRtos describe --tags --exact-match
if (($LASTEXITCODE -ne 0) -or ($Tag -ne "V11.3.0")) {
    throw "FreeRTOS dependency is not exactly V11.3.0."
}
Write-Host "[PASS] STM32CubeF4 present; FreeRTOS-Kernel $Tag selected"
