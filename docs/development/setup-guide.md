# MS3DMaker — Cross-platform setup guide

This guide reflects the **verified Windows Phase 0 install** and the matching **Linux installer**. Target stack: **C++ / Qt 6.8.3 / OCCT 7.9.3 / CMake / Ninja / Git**.

Repository: https://github.com/mauropn/MS.3DMaker

## Lessons learned (Windows) → apply everywhere

| Lesson | Why it mattered | Guidance |
|--------|-----------------|----------|
| Build OCCT on a **fast disk (SSD)** | Header collection on an HDD took tens of minutes and stalled productivity | Set `MS3D_DEV_ROOT` to an SSD path on Linux; on Windows keep build/install on C: SSD |
| **aqtinstall can fail** | Mirror SSL errors and nested `download.qt.io` package layout (`…/qt6_683/qt.qt6.683.…/{stamp}{archive}.7z`) | Prefer aqt; always keep a **manual curl + py7zr** fallback |
| Do **not** use FreeCAD/KiCad OCCT DLLs | Runtime DLLs without SDK headers/libs | Always install/build a real OCCT SDK |
| MSVC tools are **not** on default PATH | `cl` only after VsDevCmd | Use `scripts/dev-shell.ps1` (Windows) or `scripts/dev-shell.sh` (Linux) |
| REA needs a **new enough Node** | `rea-agents@6.8.0` requires `^22.23.4 \|\| ^24.22.1 \|\| >=26.12.1` | Node 24.14.0 was rejected; **24.22.1** worked |
| Smoke before “ready” | Toolchain gaps show up at link/run time | Run Qt, OCCT, and Qt+OCCT smokes |
| GitHub SSH may need host keys | First `git@github.com` push failed host verification | HTTPS + `gh auth setup-git` is a reliable fallback |

Pinned versions used on Windows (reproduce these on Linux when possible):

- Qt **6.8.3** (open-source / LGPL) + `qttools` (`lupdate` / `lrelease`)
- OCCT **V7_9_3** (shared libs, Draw/docs/optional deps off)
- CMake **4.x**, Ninja **1.13+**
- GitHub CLI latest

## Windows (already verified on founder machine)

See [`environment-readiness.md`](environment-readiness.md) for the full report.

Quick path:

```powershell
# Refresh PATH if tools were just installed
$env:Path = [System.Environment]::GetEnvironmentVariable("Path","Machine") + ";" +
            [System.Environment]::GetEnvironmentVariable("Path","User")

.\scripts\dev-shell.ps1
.\scripts\run_smokes.ps1
```

Key locations used:

| Component | Path |
|-----------|------|
| Qt | `F:\Dev\Qt\6.8.3\msvc2022_64` |
| OCCT install | `C:\Dev\OCCT\install` |
| OCCT sources | `F:\Dev\OCCT\src` |
| Scripts | `scripts\install_qt_manual.ps1`, `build_occt.ps1`, `dev-shell.ps1`, `run_smokes.ps1` |

## Linux (Ubuntu / Debian x86_64)

### One-shot installer

```bash
chmod +x scripts/setup-linux.sh scripts/dev-shell.sh scripts/run_smokes.sh
./scripts/setup-linux.sh --smoke
```

What it installs (unless skipped):

1. apt: `build-essential`, CMake, Ninja, Git/LFS, Python, OpenGL/XCB deps for Qt
2. Node.js 24.x (NodeSource) if REA engine requirements are not met
3. GitHub CLI (official apt repo)
4. Qt 6.8.3 `linux_gcc_64` / `gcc_64` via **aqt**, with **manual download.qt.io fallback**
5. OCCT `V7_9_3` clone + Ninja Release build/install under `$MS3D_DEV_ROOT/OCCT`
6. Writes `$MS3D_DEV_ROOT/ms3d-env.sh` and optionally runs smokes

### Useful flags / env

```bash
MS3D_DEV_ROOT=/mnt/ssd/ms3d-dev ./scripts/setup-linux.sh   # put SDKs on SSD
./scripts/setup-linux.sh --skip-occt                       # Qt/tools only
./scripts/setup-linux.sh --smoke-only                      # after env exists
```

### Daily development

```bash
source scripts/dev-shell.sh
# or: source ~/Dev/ms3d-env.sh

cmake --version
qmake -query QT_VERSION
test -f "$CASROOT/cmake/OpenCASCADEConfig.cmake" \
  -o -f "$CASROOT/lib/cmake/opencascade/OpenCASCADEConfig.cmake"

./scripts/run_smokes.sh
```

Default layout (`MS3D_DEV_ROOT` defaults to `$HOME/Dev`):

```text
$MS3D_DEV_ROOT/
  Qt/6.8.3/gcc_64/          # or linux_gcc_64
  OCCT/src/
  OCCT/build/
  OCCT/install/
  OCCT/CASROOT.txt
  ms3d-env.sh
```

### Sudo

`setup-linux.sh` uses `sudo` for apt and NodeSource. Qt/OCCT install under `$HOME` by default (no sudo).

### Display / CI

GUI smoke tests set `QT_QPA_PLATFORM=offscreen` when `DISPLAY` is empty so they can run headless.

## REA (optional, both platforms)

```bash
node -v   # must meet rea-agents engines
npx rea-agents@latest setup --client cursor
# Restart Cursor, then verify MCP namespace user-rea is ready
```

Authorized investigation policy: [`phase1-rea-and-investigation.md`](phase1-rea-and-investigation.md).

## Verification checklist

- [ ] `cmake` / `ninja` / C++ compiler (`cl` via VsDevCmd, or `g++`)
- [ ] Qt 6.8.x with `lupdate` and `lrelease`
- [ ] OCCT 7.9.x SDK (`OpenCASCADEConfig.cmake` + headers)
- [ ] `examples/smoke/qt_hello`, `occt_box`, `qt_occt` all exit 0
- [ ] (Optional) Node for REA; `gh` authenticated for GitHub

## Licensing reminder

Qt (open-source) and OCCT are **LGPL**-family. Comply when distributing binaries. Project application license is still pending an ADR.
