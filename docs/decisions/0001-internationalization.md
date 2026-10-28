# ADR 0001 — Internationalization with Qt Linguist

**Status:** Accepted (Phase 2)  
**Date:** 2026-10-28  
**Deciders:** Project founder (mauropn), with architectural recommendation from the coding agent  

## Problem

MS3DMaker must ship with English (`en`) and Brazilian Portuguese (`pt-BR`) from the first user-facing UI, allow more languages later without rewriting modeling code, and forbid hard-coded UI strings outside a centralized translation system.

## Decision

Use **Qt’s official internationalization stack only**:

- Source strings via `tr()` / `QCoreApplication::translate()` (English canonical text)
- Catalogs: `translations/MS3DMaker_en.ts`, `translations/MS3DMaker_pt_BR.ts`
- Compiled `.qm` files via `lrelease`, loaded with `QTranslator`
- Preferences store locale id (`en`, `pt-BR`) in `QSettings`
- Default language on first launch: **English**
- Missing translation → show English source text (never empty labels)
- Prefer **runtime language switch** via `QEvent::LanguageChange` without restart
- CAD document units stay independent of UI language; numeric serialization stays locale-independent

## Alternatives considered

| Option | Why not |
|--------|---------|
| `if (lang == "pt-BR")` conditionals | Unmaintainable; violates project rules |
| gettext / ICU message catalogs | Second framework; Qt already required |
| Separate UI per language | Duplicate work; no shared modeling code |

## Benefits

- Aligns with Qt Widgets / future Qt tooling
- Contributor workflow via Linguist is well documented
- CMake can run `lupdate` / `lrelease`
- Runtime switch preserves open documents when UI retranslates

## Disadvantages / risks

- Contributors must remember `tr()` for every new UI string
- Some third-party widgets may need explicit retranslate hooks
- `.ts` merge conflicts possible in PRs — mitigate with small string PRs and glossary

## Compatibility

- Qt 6.8+ (verified 6.8.3 on Windows)
- Works on Windows and Linux with the same catalog files

## Licensing

- Qt Linguist tools ship with open-source Qt; no extra i18n license

## Testing strategy

- Unit/integration: load `en` and `pt-BR` translators; persist preference; English fallback
- Manual: switch language in Preferences; menus/labels update; document (when present) stays open
- Future CI: `lupdate` freshness check; unfinished translation report

## Reversibility

High for catalog file layout; medium if a second framework were introduced later (not planned).

## Consequences

All new UI features are incomplete until new strings use `tr()`/`translate()` and appear in both catalogs (or are intentionally English-only pending translation, with safe fallback).
