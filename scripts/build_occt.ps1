# Configure and build OCCT V7_9_3 (minimal SDK) into F:\Dev\OCCT\install
$ErrorActionPreference = "Stop"

$VsDevCmd = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat"
if (-not (Test-Path $VsDevCmd)) {
    throw "VsDevCmd.bat not found: $VsDevCmd"
}

# Sources may live on F: (HDD). Build/install on C: (SSD) — F: header collect was prohibitively slow.
$Src = "F:\Dev\OCCT\src"
$Build = "C:\Dev\OCCT\build"
$Install = "C:\Dev\OCCT\install"
New-Item -ItemType Directory -Force -Path $Build | Out-Null
New-Item -ItemType Directory -Force -Path $Install | Out-Null
# Keep a pointer on F: for discoverability
New-Item -ItemType Directory -Force -Path "F:\Dev\OCCT" | Out-Null
Set-Content -Path "F:\Dev\OCCT\CASROOT.txt" -Value $Install -Encoding ASCII

$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" +
            [System.Environment]::GetEnvironmentVariable("Path","User")

$cmakeArgs = @(
    "-S `"$Src`"",
    "-B `"$Build`"",
    "-G Ninja",
    "-DCMAKE_BUILD_TYPE=Release",
    "-DCMAKE_INSTALL_PREFIX=`"$Install`"",
    "-DBUILD_LIBRARY_TYPE=Shared",
    "-DBUILD_MODULE_Draw=OFF",
    "-DBUILD_DOC_Overview=OFF",
    "-DUSE_FREETYPE=OFF",
    "-DUSE_FREEIMAGE=OFF",
    "-DUSE_RAPIDJSON=OFF",
    "-DUSE_TCL=OFF",
    "-DUSE_TK=OFF",
    "-DUSE_VTK=OFF",
    "-DUSE_TBB=OFF",
    "-DUSE_OPENGL=ON",
    "-DUSE_GLES2=OFF"
) -join " "

$buildCmd = "cmake --build `"$Build`" --parallel && cmake --install `"$Build`""

$bat = @"
@echo off
call "$VsDevCmd" -arch=amd64 -host_arch=amd64
if errorlevel 1 exit /b 1
cmake $cmakeArgs
if errorlevel 1 exit /b 1
$buildCmd
exit /b %ERRORLEVEL%
"@

$batPath = Join-Path $env:TEMP "build_occt_ms3d.bat"
Set-Content -Path $batPath -Value $bat -Encoding ASCII
Write-Host "Running $batPath"
& cmd /c $batPath
exit $LASTEXITCODE
