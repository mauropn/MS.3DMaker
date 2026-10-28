# MS3DMaker

Independent open-source parametric 3D CAD application (C++ / Qt / OCCT).

**Founder / lead:** [mauropn](https://github.com/mauropn)  
**Repository:** https://github.com/mauropn/MS.3DMaker

## Status

- Phase 0 — Windows toolchain verified; Linux installer script available  
- Phase 1 — REA MCP configured; git remote connected  
- Phase 2 — Internationalization foundation (`en` + `pt-BR`)  

## Quick start (Windows)

See [docs/development/setup-guide.md](docs/development/setup-guide.md) and [docs/development/environment-readiness.md](docs/development/environment-readiness.md).

```powershell
.\scripts\dev-shell.ps1
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=F:/Dev/Qt/6.8.3/msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
.\build\MS3DMaker.exe
```

## Quick start (Linux)

```bash
./scripts/setup-linux.sh --smoke
source scripts/dev-shell.sh
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Translations

Every user-facing string must use Qt `tr()` / `translate()`. See [translations/CONTRIBUTING_TRANSLATIONS.md](translations/CONTRIBUTING_TRANSLATIONS.md).

## License

Application license not yet locked (ADR pending). Qt and OCCT are LGPL-family dependencies.
