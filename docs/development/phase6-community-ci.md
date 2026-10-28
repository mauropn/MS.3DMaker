# Phase 6 — Community development and CI

## Goal

Prepare MS3DMaker for external contributions: docs, conventions, quality gates, and CI that builds/tests without publishing binaries.

## Delivered

| Item | Location |
|------|----------|
| Architecture overview | [docs/architecture.md](../architecture.md) |
| Coding conventions | [coding-conventions.md](coding-conventions.md) |
| Roadmap | [docs/ROADMAP.md](../ROADMAP.md) |
| Release process (manual) | [docs/RELEASE.md](../RELEASE.md) |
| clang-format | [`.clang-format`](../../.clang-format) |
| Translation catalog check | `scripts/check_translations.py` |
| Format check | `scripts/check_clang_format.py` |
| GitHub Actions | `.github/workflows/ci.yml` |

## CI jobs

1. **quality** — translation unfinished/empty checks; clang-format dry-run on `src/` + `tests/`.  
2. **linux** — Qt 6.8 via aqt, OCCT `V7_9_3` (cached install), CMake Ninja Release, `ctest`.  
3. **windows** — same stack on `windows-latest` with MSVC, cached OCCT.

Secrets are not printed. Workflows do **not** create releases or upload binaries ([RELEASE.md](../RELEASE.md)).

### Publishing the workflow file

GitHub rejects pushes of `.github/workflows/*` unless the credential has the **`workflow`** OAuth scope. If `git push` fails with that message:

```powershell
gh auth refresh -h github.com -s workflow
git push origin main
```

## Verify locally

```bash
python scripts/check_translations.py
python scripts/check_clang_format.py
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```
