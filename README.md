# MS3DMaker

Independent open-source parametric 3D CAD application (C++ / Qt / OCCT).

**Founder / lead:** [mauropn](https://github.com/mauropn)  
**Repository:** https://github.com/mauropn/MS.3DMaker

## Status

- Phase 0 — Windows toolchain verified; Linux installer script available  
- Phase 1 — REA MCP configured; git remote connected  
- Phase 2 — Internationalization foundation (`en` + `pt-BR`)  
- Phase 3 — Governance docs published; **application license Proposed** (see below)

## Community

- [GOVERNANCE.md](GOVERNANCE.md) — founder-led project  
- [CONTRIBUTING.md](CONTRIBUTING.md) — how to contribute  
- [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) · [SECURITY.md](SECURITY.md) · [SUPPORT.md](SUPPORT.md)  
- [CHANGELOG.md](CHANGELOG.md) · [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)

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

Application license is **not locked yet**. Trade-offs and the recommended choice (**Apache-2.0**) are in [docs/decisions/0002-project-license.md](docs/decisions/0002-project-license.md). Qt and OCCT remain LGPL-family dependencies regardless of the application license — see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
