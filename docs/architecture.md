# MS3DMaker architecture

High-level layout for contributors. Details live under `docs/development/` and `docs/decisions/`.

## Stack

| Layer | Choice |
|-------|--------|
| Language | C++17 |
| UI | Qt 6 Widgets (`tr()` / Linguist; `en` + `pt-BR`) |
| Kernel | Open CASCADE Technology (OCCT) 7.9.x |
| Build | CMake + Ninja |
| Docs / governance | Markdown in-repo; founder-led |

## Source map

```
src/
  app/           Entry point
  ai/            Constrained AI Assist (plan → validate → confirm → execute)
  assembly/      Component instances + transforms
  document/      .ms3d schema v1 load/save
  drawing/       HLR drawing views
  features/      FeatureHistory (params, deps, recompute)
  geometry/      SolidFactory primitives / booleans / extrude / revolve
  i18n/          LanguageService
  io/            STEP import/export
  sketch/        Sketch planes, entities, constraints
  ui/            MainWindow and dialogs
  viewport/      OcctView (display, navigation, selection)
```

## Modeling pipeline

1. **Sketch / SolidFactory** — create topology.  
2. **FeatureHistory** — record ops with parameters and dependencies; `recompute()` rebuilds tip.  
3. **Document** — serialize feature graph to `.ms3d`.  
4. **STEP / Assembly / Drawing** — interchange and presentation sessions.  
5. **AI Assist** — `AiPlanner` emits `AiPlan`; `AiValidator` checks; UI confirms; `AiExecutor` calls FeatureHistory only (see [ADR 0004](decisions/0004-ai-assist-safety.md)).

## Safety boundaries

- No LLM writes to arbitrary memory or unrestricted code paths.  
- Consequential AI ops require preview + confirmation.  
- Dual license **MIT OR Apache-2.0**; Qt/OCCT remain LGPL-family (see `THIRD_PARTY_NOTICES.md`).

## Tests

Qt Test binaries under `tests/` (`ctest`). Prefer covering new behavior with a focused test target rather than UI automation.
