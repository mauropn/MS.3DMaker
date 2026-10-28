# Contributing translations

## Rules

1. **Never** put user-facing text in UI code without `tr()` or `QCoreApplication::translate()`.
2. English source strings inside `tr("...")` are the canonical text.
3. Do **not** use `if (language == "pt-BR")` for UI labels.
4. Keep CAD identifiers, serialization keys, and API names untranslated.

## Workflow

1. Add or change UI strings using `tr("English text")`.
2. Refresh catalogs, then compile `.qm` files:

   ```bash
   # Preferred when lupdate works:
   cmake --build build --target ms3d_lupdate
   cmake --build build --target ms3d_lrelease
   ```

   **Windows note:** on the verified Qt 6.8.3 MSVC kit, `lupdate.exe` may fail to start
   (`0xC0000135`). In that case, edit the `.ts` files directly (or use Linguist if it
   launches) and rebuild so CMake’s `lrelease` step regenerates `.qm` files.

3. Open `MS3DMaker_pt_BR.ts` in Qt Linguist and translate unfinished entries.
4. The English catalog may remain mostly “finished” as identity translations or source-only.
5. Rebuild so `.qm` files refresh; test both languages in Preferences.
6. To add a locale (e.g. `es`): add `MS3DMaker_es.ts`, register it in CMake and `LanguageService`, translate, test.

## Checks before merge

- No new hard-coded UI strings in changed UI files
- `pt-BR` has no empty translations for new strings (or document intentional deferral)
- Runtime switch and English fallback still work
