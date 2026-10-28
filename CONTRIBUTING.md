# Contributing to MS3DMaker

Thank you for your interest in contributing. MS3DMaker is founder-led; all contributions are reviewed before merge.

## Before you start

1. Read [GOVERNANCE.md](GOVERNANCE.md) and [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).
2. Skim [docs/development/setup-guide.md](docs/development/setup-guide.md).
3. For UI work, follow [translations/CONTRIBUTING_TRANSLATIONS.md](translations/CONTRIBUTING_TRANSLATIONS.md) — **no hard-coded user-facing strings**.

## Development setup

- **Windows:** Phase 0 paths and `scripts/dev-shell.ps1` — see setup guide.  
- **Linux:** `./scripts/setup-linux.sh` then `source scripts/dev-shell.sh`.

Build:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## Pull requests

1. Fork or branch from `main`.
2. Keep changes focused and reviewable.
3. Include or update tests when behavior changes.
4. Update docs when behavior or setup changes.
5. Ensure new UI strings use `tr()` / `translate()` and update `translations/` catalogs.
6. Do not commit secrets, credentials, or machine-local absolute paths that are not documented as examples.
7. Fill out the pull request template.

## Issues

Use GitHub issue templates for bugs and feature requests. Mark only genuinely small, well-scoped tasks as good first issues (maintainers decide).

## License of contributions

By submitting a contribution, you agree that it may be distributed under the project's eventual public license (see [docs/decisions/0002-project-license.md](docs/decisions/0002-project-license.md)). Until `LICENSE` is locked, contributions are accepted under the understanding that they will be relicensed to the founder-chosen OSI license without additional permission hurdles.

## Security

Do not file security vulnerabilities as public issues. See [SECURITY.md](SECURITY.md).
