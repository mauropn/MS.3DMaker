# Manual Qt 6.8.3 MSVC2022_64 install (aqt mirror/SSL workaround)
$ErrorActionPreference = "Stop"

$Version = "6.8.3-0-202503201308"
# Qt online repo now nests archives as: .../qt6_683/qt.qt6.683.win64_msvc2022_64/{version}{archive}.7z
$Base = "https://download.qt.io/online/qtsdkrepository/windows_x86/desktop/qt6_683/qt6_683/qt.qt6.683.win64_msvc2022_64"
$ArchiveDir = "F:\Dev\Qt\_archives"
$ExtractRoot = "F:\Dev\Qt\6.8.3\msvc2022_64"

New-Item -ItemType Directory -Force -Path $ArchiveDir | Out-Null
New-Item -ItemType Directory -Force -Path $ExtractRoot | Out-Null

$archives = @(
    "qtbase-Windows-Windows_11_23H2-MSVC2022-Windows-Windows_11_23H2-X86_64.7z",
    "qttools-Windows-Windows_11_23H2-MSVC2022-Windows-Windows_11_23H2-X86_64.7z",
    "qtsvg-Windows-Windows_11_23H2-MSVC2022-Windows-Windows_11_23H2-X86_64.7z",
    "d3dcompiler_47-x64.7z",
    "opengl32sw-64-mesa_11_2_2-signed_sha256.7z"
)

foreach ($a in $archives) {
    $fileName = "$Version$a"
    $dest = Join-Path $ArchiveDir $fileName
    if ((Test-Path $dest) -and ((Get-Item $dest).Length -gt 1000)) {
        Write-Host "SKIP download (exists): $fileName"
        continue
    }
    $url = "$Base/$fileName"
    Write-Host "DOWNLOAD $url"
    & curl.exe -L --fail --retry 5 --retry-delay 2 -o $dest $url
    if ($LASTEXITCODE -ne 0) { throw "curl failed for $fileName exit=$LASTEXITCODE" }
    Write-Host ("OK size={0:N0}" -f (Get-Item $dest).Length)
}

Write-Host "Extracting with py7zr..."
$py = @"
from pathlib import Path
import py7zr

archive_dir = Path(r'$ArchiveDir')
extract_root = Path(r'$ExtractRoot')
bin_root = extract_root / 'bin'
extract_root.mkdir(parents=True, exist_ok=True)
bin_root.mkdir(parents=True, exist_ok=True)
for z in sorted(archive_dir.glob('*.7z')):
    name = z.name.lower()
    target = bin_root if ('d3dcompiler' in name or 'opengl32sw' in name) else extract_root
    print('EXTRACT', z.name, '->', target)
    with py7zr.SevenZipFile(z, mode='r') as archive:
        archive.extractall(path=target)
print('DONE')
"@
$pyFile = Join-Path $env:TEMP "extract_qt.py"
Set-Content -Path $pyFile -Value $py -Encoding UTF8
python $pyFile

# aqt/layout often extracts into 6.8.3/msvc2022_64 nested; normalize if needed
$binCandidate = Join-Path $ExtractRoot "bin\qmake.exe"
$alt = Get-ChildItem "F:\Dev\Qt" -Recurse -Filter "qmake.exe" -ErrorAction SilentlyContinue | Select-Object -First 3
Write-Host "qmake candidates:"
$alt | ForEach-Object { $_.FullName }

if (-not (Test-Path $binCandidate)) {
    # Sometimes archives extract with prefix 6.8.3/msvc2022_64 inside
    $nested = Get-ChildItem "F:\Dev\Qt" -Recurse -Filter "qmake.exe" -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match 'msvc2022_64\\bin\\qmake\.exe$' } |
        Select-Object -First 1
    if ($nested) {
        Write-Host "Found nested kit at $($nested.Directory.Parent.FullName)"
    }
}

Write-Host "Manual Qt install script finished."
