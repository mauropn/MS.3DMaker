# Phase 2 — Internationalization

**Status:** Complete (foundation)  
**ADR:** [`docs/decisions/0001-internationalization.md`](../decisions/0001-internationalization.md)

## Delivered

- Qt Linguist catalogs: `translations/MS3DMaker_en.ts`, `translations/MS3DMaker_pt_BR.ts`
- Glossary + contributor workflow docs under `translations/`
- `ms3d::i18n::LanguageService` — load `.qm`, persist `ui/language` in `QSettings`, English fallback
- Minimal UI shell: main window + Preferences language combo (native names)
- Runtime retranslate via `QEvent::LanguageChange`
- CMake: `MS3DMaker` app, `lrelease`, copy `.qm` next to executable, `test_language_service`

## Build / test (Windows, verified)

```powershell
$env:Path = "F:\Dev\Qt\6.8.3\msvc2022_64\bin;" + $env:Path
# from VsDevCmd / scripts/dev-shell.ps1
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=F:/Dev/Qt/6.8.3/msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
.\build\MS3DMaker.exe
```

Verified: **13** strings in both catalogs; `language_service` test **passed**.

## Known tooling issue

On this Windows Qt 6.8.3 kit, `lupdate.exe` fails to start (`0xC0000135`, missing DLL). **`lrelease` works** (including via CMake). Until `lupdate` is repaired:

1. Add/edit strings in `.ts` catalogs (or use Qt Linguist GUI if it launches)
2. Rebuild so CMake runs `lrelease`
3. Optionally reinstall/repair the `qttools` module

## Absolute UI rule

No user-facing hard-coded strings outside `tr()` / `QCoreApplication::translate()`.
