# MS3DMaker — Environment Readiness Report

**Project:** MS3DMaker (founder-led open-source CAD)  
**Phase:** 0 — Inspect and prepare development environment  
**Host:** Windows workstation of the project founder  
**Phase 0 status:** **COMPLETE** (toolchain installed; smoke tests passed)  
**Cross-platform setup:** see [`setup-guide.md`](setup-guide.md) (includes Linux `scripts/setup-linux.sh`)

## 1. Machine profile (verified)

| Item | Value |
|------|--------|
| OS | Microsoft Windows NT 10.0.26300.0 (x64) |
| CPU | Intel Core 5 210H (8 cores / 12 logical) |
| RAM | ~31.6 GB |
| Workspace | `f:\Mauro Sistemas\Projetos\MS3DMaker` |
| Disk | **C:** SSD (~544 GB free) — preferred for builds/SDKs; **F:** HDD (~856 GB free) — workspace + large archives |

## 2. Installed toolchain (verified on Windows)

| Dependency | Version / location | License notes | Verification |
|------------|-------------------|---------------|--------------|
| Git + LFS | 2.53.0 / 3.7.1 | — | `git --version` |
| MSVC | 19.50.35726 (VS 2026 Build Tools 18.4.0) | Microsoft EULA | Via `scripts\dev-shell.ps1` / VsDevCmd |
| Windows SDK | 10.0.26100.0 | — | Present |
| CMake | **4.4.4** (Kitware, winget) | BSD-3 | `cmake --version` |
| Ninja | **1.13.2** (winget) | Apache-2.0 | `ninja --version` |
| GitHub CLI | **2.103.3** (winget) | MIT | `gh --version` |
| Qt | **6.8.3** MSVC2022_64 at `F:\Dev\Qt\6.8.3\msvc2022_64` | LGPL (open-source) | `qmake -query QT_VERSION` → 6.8.3; `lupdate`/`lrelease` present |
| OCCT | **7.9.3** at `C:\Dev\OCCT\install` | LGPL + exception | `OpenCASCADEConfig.cmake`; `TKernel.dll` |
| Python | 3.14.3 / 3.12 | — | Used for aqt/manual Qt extract |
| Node.js | **v24.22.1** | — | Required by REA (`^24.22.1`) |
| REA | `rea-agents@6.8.0` MCP (`user-rea`) | — | Verified ready; see `phase1-rea-and-investigation.md` |
| GitHub remote | https://github.com/mauropn/MS.3DMaker.git | — | Local `main` tracks `origin/main` |

### Path notes (Windows)

- **Qt** under `F:\Dev\Qt` (manual download/extract; aqt failed on mirror SSL / nested repo layout). Script: `scripts/install_qt_manual.ps1`.
- **OCCT sources** on `F:\Dev\OCCT\src` (tag `V7_9_3`). **Build + install on C: SSD** (`C:\Dev\OCCT\build`, `C:\Dev\OCCT\install`) because header collection on F: HDD was prohibitively slow. Pointer: `F:\Dev\OCCT\CASROOT.txt`.
- FreeCAD/KiCad OCCT DLLs remain **unused** (no SDK headers).

### Linux (not yet run on founder hardware)

Use `scripts/setup-linux.sh` — mirrors the pinned versions and Windows lessons (SSD `MS3D_DEV_ROOT`, aqt + manual Qt fallback, OCCT `V7_9_3`, smoke tests). Full instructions: [`setup-guide.md`](setup-guide.md).

## 3. Developer shell

**Windows**

```powershell
.\scripts\dev-shell.ps1
.\scripts\dev-shell.ps1 -Command "cmake --version"
```

**Linux**

```bash
source scripts/dev-shell.sh
# or: source ~/Dev/ms3d-env.sh
```

## 4. Smoke tests (passed on Windows)

```powershell
.\scripts\run_smokes.ps1
```

```bash
./scripts/run_smokes.sh
```

| Test | Path | Windows result |
|------|------|----------------|
| Qt Widgets hello | `examples/smoke/qt_hello` | **OK** |
| OCCT BRep box | `examples/smoke/occt_box` | **OK** — `box solid count=1` |
| Qt + OCCT | `examples/smoke/qt_occt` | **OK** |

Logs: `docs/development/smoke-logs/`.

## 5. Post-install verification log (Windows)

```
cmake version 4.4.4
ninja 1.13.2
gh version 2.103.3
qmake QT_VERSION 6.8.3
OCCT 7.9.3 (C:\Dev\OCCT\install)
MSVC 19.50.35726 via VsDevCmd
Node v24.22.1
All smoke tests passed (scripts/run_smokes.ps1)
```

## 6. Scripts

| Script | Purpose |
|--------|---------|
| `scripts/dev-shell.ps1` | Windows: MSVC + Qt + OCCT environment |
| `scripts/dev-shell.sh` | Linux: Qt + OCCT environment |
| `scripts/setup-linux.sh` | Linux: apt/Node/gh/Qt/OCCT installer |
| `scripts/run_smokes.ps1` / `run_smokes.sh` | Build and run smoke examples |
| `scripts/aqt-settings.ini` | aqt mirror/timeout config |
| `scripts/install_qt_manual.ps1` | Windows Qt 6.8.3 manual download |
| `scripts/build_occt.ps1` | Windows OCCT 7.9.3 build/install |
| `scripts/parse_qt_updates.py` | Helper for Qt Updates.xml |

## 7. Licensing reminders

- Distributing binaries that link **Qt** and **OCCT** requires complying with their **LGPL** terms.
- Project application `LICENSE` is **not** locked — ADR before treating licensing as final.
- Recommendation to discuss: Apache-2.0 or MIT for the app, with third-party notices.

## 8. Phase status

| Phase | Status |
|-------|--------|
| 0 Environment | **Complete** (Windows verified; Linux script added) |
| 1 REA + git remote | **Complete** (MCP `user-rea` ready; repo linked) |
| 2 i18n foundation | **Complete** — see `phase2-i18n.md` + ADR 0001 |
| 3 Governance + license | **Complete** — MIT OR Apache-2.0 (SolidCraft-style dual license) |
| 4 First CAD prototype | Pending founder approval |
