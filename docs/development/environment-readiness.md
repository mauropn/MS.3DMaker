# MS3DMaker — Environment Readiness Report

**Project:** MS3DMaker (founder-led open-source CAD)  
**Phase:** 0 — Inspect and prepare development environment  
**Host:** Windows workstation of the project founder  
**Phase 0 status:** **COMPLETE** (toolchain installed; smoke tests passed)

## 1. Machine profile (verified)

| Item | Value |
|------|--------|
| OS | Microsoft Windows NT 10.0.26300.0 (x64) |
| CPU | Intel Core 5 210H (8 cores / 12 logical) |
| RAM | ~31.6 GB |
| Workspace | `f:\Mauro Sistemas\Projetos\MS3DMaker` |
| Disk | **C:** SSD (~544 GB free) — preferred for builds/SDKs; **F:** HDD (~856 GB free) — workspace + large archives |

## 2. Installed toolchain (verified)

| Dependency | Version / location | License notes | Verification |
|------------|-------------------|---------------|--------------|
| Git + LFS | 2.53.0 / 3.7.1 | — | `git --version` |
| MSVC | 19.50.35726 (VS 2026 Build Tools 18.4.0) | Microsoft EULA | Via `scripts\dev-shell.ps1` / VsDevCmd |
| Windows SDK | 10.0.26100.0 | — | Present |
| CMake | **4.4.4** (Kitware, winget) | BSD-3 | `cmake --version` |
| Ninja | **1.13.2** (winget) | Apache-2.0 | `ninja --version` |
| GitHub CLI | **2.103.3** (winget) | MIT | `gh --version` (auth not done) |
| Qt | **6.8.3** MSVC2022_64 at `F:\Dev\Qt\6.8.3\msvc2022_64` | LGPL (open-source) | `qmake -query QT_VERSION` → 6.8.3; `lupdate`/`lrelease` present |
| OCCT | **7.9.3** at `C:\Dev\OCCT\install` | LGPL + exception | `OpenCASCADEConfig.cmake`; `TKernel.dll` |
| Python | 3.14.3 / 3.12 | — | Used for aqt/manual Qt extract |
| Node / npm | 24.14.0 / 11.9.0 | — | For Phase 1 REA |
| REA | `rea-agents@6.8.0` MCP in Cursor | — | Configured Phase 1; **restart Cursor** to load; see `phase1-rea-and-investigation.md` |
| Node.js | **v24.22.1** | — | Required by REA (`^24.22.1`) |
| GitHub remote | https://github.com/mauropn/MS.3DMaker.git | — | Exists; local folder not yet a git clone/link |

### Path notes

- **Qt** installed under `F:\Dev\Qt` (manual download/extract; aqt failed on mirror SSL / nested repo layout). Script: `scripts/install_qt_manual.ps1`.
- **OCCT sources** cloned to `F:\Dev\OCCT\src` (tag `V7_9_3`). **Build + install on C: SSD** (`C:\Dev\OCCT\build`, `C:\Dev\OCCT\install`) because header collection on F: HDD was prohibitively slow. Pointer file: `F:\Dev\OCCT\CASROOT.txt`.
- FreeCAD/KiCad OCCT DLLs remain **unused** (no SDK headers).

## 3. Developer shell

```powershell
.\scripts\dev-shell.ps1
# or one-shot:
.\scripts\dev-shell.ps1 -Command "cmake --version"
```

Loads VsDevCmd (MSVC x64), then prepends Qt and OCCT bin dirs when present.

## 4. Smoke tests (passed)

Run:

```powershell
.\scripts\run_smokes.ps1
```

| Test | Path | Result |
|------|------|--------|
| Qt Widgets hello | `examples/smoke/qt_hello` | **OK** (exit 0) |
| OCCT BRep box | `examples/smoke/occt_box` | **OK** — `MS3DMaker OCCT smoke OK: box solid count=1` |
| Qt + OCCT link/UI | `examples/smoke/qt_occt` | **OK** (exit 0) |

Logs: `docs/development/smoke-logs/`.

## 5. Post-install verification log

```
cmake version 4.4.4
ninja 1.13.2
gh version 2.103.3
qmake QT_VERSION 6.8.3
OCCT 7.9.3 (C:\Dev\OCCT\install)
MSVC 19.50.35726 via VsDevCmd
All smoke tests passed (scripts/run_smokes.ps1)
```

## 6. Scripts added in Phase 0

| Script | Purpose |
|--------|---------|
| `scripts/dev-shell.ps1` | MSVC + Qt + OCCT environment |
| `scripts/aqt-settings.ini` | aqt mirror/timeout config (reference) |
| `scripts/install_qt_manual.ps1` | Download/extract Qt 6.8.3 MSVC kit |
| `scripts/build_occt.ps1` | Configure/build/install OCCT 7.9.3 |
| `scripts/run_smokes.ps1` | Build and run smoke examples |
| `scripts/parse_qt_updates.py` | Helper for Qt Updates.xml |

## 7. Licensing reminders (not locked for the app)

- Distributing MS3DMaker binaries that link **Qt** and **OCCT** requires complying with their **LGPL** terms.
- Project `LICENSE` is **not** chosen yet — prepare an ADR before public GitHub publish.
- Recommendation to discuss next: Apache-2.0 or MIT for the application, with third-party notices.

## 8. Next phases (awaiting founder approval)

1. **Phase 1** — REA setup (`npx rea-agents@latest setup`); authorized investigation only; no Fusion decompilation.
2. **Phase 2** — i18n architecture ADR (`tr()`, `.ts` / `.qm`, runtime language switch).
3. **Phase 3** — License ADR + GitHub governance (`gh auth login` first).
4. **Phase 4** — First CAD prototype (viewport + box + en/pt-BR).

Do **not** scaffold the full CAD application until you approve the next phase.
