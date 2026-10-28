# MS3DMaker Windows developer shell
# Loads Visual Studio Build Tools (MSVC x64) and optional Qt / OCCT paths.
# Usage: .\scripts\dev-shell.ps1
#        .\scripts\dev-shell.ps1 -Command "cmake --version"

[CmdletBinding()]
param(
    [string]$Command = ""
)

$ErrorActionPreference = "Stop"

$VsDevCmdCandidates = @(
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat",
    "${env:ProgramFiles}\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat",
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
)

$VsDevCmd = $VsDevCmdCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $VsDevCmd) {
    Write-Error "VsDevCmd.bat not found. Install Visual Studio Build Tools with the C++ workload."
}

# Optional SDK roots (Phase 0 defaults)
if (-not $env:QT_ROOT -and (Test-Path "F:\Dev\Qt")) {
    $qtKits = Get-ChildItem "F:\Dev\Qt" -Directory -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -match '^\d+\.\d+' } |
        Sort-Object Name -Descending
    foreach ($ver in $qtKits) {
        $msvcKit = Get-ChildItem $ver.FullName -Directory -ErrorAction SilentlyContinue |
            Where-Object { $_.Name -match 'msvc' } |
            Select-Object -First 1
        if ($msvcKit) {
            $env:QT_ROOT = $msvcKit.FullName
            break
        }
    }
}
if ($env:QT_ROOT) {
    $env:CMAKE_PREFIX_PATH = $env:QT_ROOT
    $env:PATH = "$(Join-Path $env:QT_ROOT 'bin');$env:PATH"
}

if (-not $env:CASROOT) {
    if (Test-Path "C:\Dev\OCCT\install") {
        $env:CASROOT = "C:\Dev\OCCT\install"
    } elseif (Test-Path "F:\Dev\OCCT\CASROOT.txt") {
        $env:CASROOT = (Get-Content "F:\Dev\OCCT\CASROOT.txt" -Raw).Trim()
    } elseif (Test-Path "F:\Dev\OCCT\install") {
        $env:CASROOT = "F:\Dev\OCCT\install"
    }
}
if ($env:CASROOT) {
    $occtBin = Join-Path $env:CASROOT "win64\vc14\bin"
    if (-not (Test-Path $occtBin)) {
        $occtBin = Join-Path $env:CASROOT "bin"
    }
    if (Test-Path $occtBin) {
        $env:PATH = "$occtBin;$env:PATH"
    }
    $env:CSF_OCCTIncludePath = Join-Path $env:CASROOT "inc"
    $env:CSF_OCCTLibPath = Join-Path $env:CASROOT "win64\vc14\lib"
    if (-not (Test-Path $env:CSF_OCCTLibPath)) {
        $env:CSF_OCCTLibPath = Join-Path $env:CASROOT "lib"
    }
}

Write-Host "MS3DMaker dev shell"
Write-Host "  VsDevCmd : $VsDevCmd"
if ($env:QT_ROOT) { Write-Host "  QT_ROOT  : $env:QT_ROOT" }
if ($env:CASROOT) { Write-Host "  CASROOT  : $env:CASROOT" }

$inner = 'call "{0}" -arch=amd64 -host_arch=amd64 >nul &&' -f $VsDevCmd
if ($Command) {
    cmd /c "$inner $Command"
    exit $LASTEXITCODE
}

cmd /k "$inner echo Dev environment ready. && powershell -NoLogo -NoExit"
