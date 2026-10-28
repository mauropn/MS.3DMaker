# Contributing to MS3DMaker

Thank you for your interest in contributing. MS3DMaker is founder-led; all contributions are reviewed before merge.

## Before you start

1. Read [GOVERNANCE.md](GOVERNANCE.md) and [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
2. Skim [docs/development/setup-guide.md](docs/development/setup-guide.md).
3. For UI work, follow [translations/CONTRIBUTING_TRANSLATIONS.md](translations/CONTRIBUTING_TRANSLATIONS.md) — **no hard-coded user-facing strings**.

## Development setup

- **Windows:** Phase 0 paths and `scripts/dev-shell.ps1` — see setup guide.  
- **Linux:** `./scripts/setup-linux.sh` then `source scripts/dev-shell.sh`.  
- Architecture: [docs/architecture.md](docs/architecture.md) · conventions: [docs/development/coding-conventions.md](docs/development/coding-conventions.md)

Build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Quality gates (also run in CI):

```bash
python scripts/check_translations.py
python scripts/check_clang_format.py
```

## Pull requests

1. Fork or branch from `main`.
2. Keep changes focused and reviewable.
3. Include or update tests when behavior changes.
4. Update docs when behavior or setup changes.
5. Ensure new UI strings use `tr()` / `translate()` and update `translations/` catalogs.
6. Do not commit secrets, credentials, or machine-local absolute paths that are not documented as examples.
7. Fill out the pull request template.
8. Wait for GitHub Actions CI (quality + Linux/Windows build) before merge when possible.

## Issues

Use GitHub issue templates for bugs and feature requests. Mark only genuinely small, well-scoped tasks as good first issues (maintainers decide).

## License of contributions

Unless you explicitly state otherwise, any contribution intentionally submitted for inclusion in MS3DMaker is dual-licensed under **MIT OR Apache-2.0** (see [LICENSE](LICENSE)), without additional terms.

## Security

Do not file security vulnerabilities as public issues. See [SECURITY.md](SECURITY.md).
