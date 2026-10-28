# Phase 5.4 — Milestone 5: Document management

## Goal

New / Open / Save / Save As for `.ms3d` JSON documents with schema versioning, corruption detection, and refusal to silently overwrite incompatible files.

## Format

- Extension: `.ms3d`
- `format`: `MS3DMaker`
- `schemaVersion`: `1` (current)
- Payload: title, feature history (params + deps, not B-Rep), sketch entities/constraints/dimensions

Solids are **recomputed** after load.

## Compatibility rules

| Situation | Behaviour |
|-----------|-----------|
| Corrupt JSON / missing fields | Open refused |
| Wrong `format` | Open refused |
| `schemaVersion` newer than app | Open refused; Save As will not overwrite that file |
| Older supported schema | Migrate in loader (none yet besides v1) |
| Unknown older schema | Open refused |

Never partially apply a failed load into the in-memory document.

## UI

**File → New / Open… / Save / Save As…**; dirty `*` in the window title; prompt on close/new/open.
