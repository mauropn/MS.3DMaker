# Coding conventions

## C++

- C++17; prefer clear names over clever templates.  
- Namespaces: `ms3d::<area>` (e.g. `ms3d::features`, `ms3d::ai`).  
- Headers next to sources under `src/`.  
- Prefer RAII; avoid raw owning pointers for new code.  
- OCCT handles (`TopoDS_Shape`, `Handle(…)`) stay at geometry/feature boundaries — do not leak into UI string formatting.

## Formatting

- Style file: [`.clang-format`](../../.clang-format) (based on LLVM, 4-space indent, 100-column soft limit).  
- Check locally: `python scripts/check_clang_format.py`  
- CI runs the same check on `src/` and `tests/`.

## Internationalization

- Every user-facing string: `tr()` / `QCoreApplication::translate()`.  
- Update `translations/MS3DMaker_en.ts` and `MS3DMaker_pt_BR.ts`.  
- See [translations/CONTRIBUTING_TRANSLATIONS.md](../../translations/CONTRIBUTING_TRANSLATIONS.md).

## Tests and docs

- Behavior changes → update or add a `tests/test_*.cpp` target.  
- User-visible or setup changes → update the matching `docs/development/` note and `CHANGELOG.md`.

## Security / hygiene

- Never commit secrets, tokens, or private machine paths as defaults (examples in docs are OK if labeled).  
- Do not log credentials in CI (see [SECURITY.md](../../SECURITY.md)).
