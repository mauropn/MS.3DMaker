# Build and run Phase 0 smoke tests (requires VsDevCmd / MSVC, Qt, OCCT)
$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $PSScriptRoot
$VsDevCmd = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat"
$QtRoot = if ($env:QT_ROOT) { $env:QT_ROOT } else { "F:\Dev\Qt\6.8.3\msvc2022_64" }
$CasRoot = if ($env:CASROOT) { $env:CASROOT } elseif (Test-Path "C:\Dev\OCCT\install") { "C:\Dev\OCCT\install" } else { "F:\Dev\OCCT\install" }

$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" +
            [System.Environment]::GetEnvironmentVariable("Path","User") + ";" +
            (Join-Path $QtRoot "bin") + ";" +
            (Join-Path $CasRoot "win64\vc14\bin") + ";" +
            (Join-Path $CasRoot "bin")

$env:QT_ROOT = $QtRoot
$env:CASROOT = $CasRoot
$env:CMAKE_PREFIX_PATH = $QtRoot

function Invoke-DevCmd([string]$Inner) {
    # Append Qt/OCCT after VsDevCmd so MSVC tools remain on PATH.
    $bat = @"
@echo off
call "$VsDevCmd" -arch=amd64 -host_arch=amd64 >nul
set "PATH=$QtRoot\bin;$CasRoot\win64\vc14\bin;%PATH%"
set "QT_ROOT=$QtRoot"
set "CASROOT=$CasRoot"
set "CMAKE_PREFIX_PATH=$QtRoot"
$Inner
exit /b %ERRORLEVEL%
"@
    $p = Join-Path $env:TEMP "ms3d_smoke_step.bat"
    Set-Content -Path $p -Value $bat -Encoding ASCII
    & cmd /c $p
    if ($LASTEXITCODE -ne 0) { throw "Command failed ($LASTEXITCODE): $Inner" }
}

$smokes = @(
    @{ Name = "qt_hello"; Dir = Join-Path $Root "examples\smoke\qt_hello" },
    @{ Name = "occt_box"; Dir = Join-Path $Root "examples\smoke\occt_box" },
    @{ Name = "qt_occt"; Dir = Join-Path $Root "examples\smoke\qt_occt" }
)

$logDir = Join-Path $Root "docs\development\smoke-logs"
New-Item -ItemType Directory -Force -Path $logDir | Out-Null

foreach ($s in $smokes) {
    $build = Join-Path $s.Dir "build"
    Write-Host "=== Building $($s.Name) ==="
    Invoke-DevCmd "cmake -S `"$($s.Dir)`" -B `"$build`" -G Ninja -DCMAKE_BUILD_TYPE=Release -DOpenCASCADE_DIR=`"$CasRoot/cmake`" && cmake --build `"$build`""
    $exe = Join-Path $build "$($s.Name).exe"
    Write-Host "=== Running $($s.Name) ==="
    $outLog = Join-Path $logDir "$($s.Name).out.txt"
    $errLog = Join-Path $logDir "$($s.Name).err.txt"
    & $exe 1> $outLog 2> $errLog
    if ($LASTEXITCODE -ne 0) { throw "$($s.Name) failed with exit $LASTEXITCODE" }
    Write-Host "$($s.Name) OK"
}

Write-Host "All smoke tests passed."
