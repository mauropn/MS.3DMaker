# ADR 0004 — AI assist safety model

**Status:** Accepted  
**Date:** 2026-10-28  
**Deciders:** Project founder ([mauropn](https://github.com/mauropn))

## Problem

Milestone 8 needs natural-language assisted modeling without letting an LLM mutate process memory or run unrestricted code.

## Decision

1. **Structured plans only** — prompts become `AiPlan` / `AiStep` records.  
2. **Constrained planner** — in-tree recipe/regex planner (`planFromPrompt`); no live LLM required for MVP.  
3. **Validate then confirm** — `validatePlan` + UI preview + explicit user confirmation.  
4. **Execute through FeatureHistory only** — `executePlan` calls `addPrimitive` / `addBoolean` / `recompute`.  
5. Application chrome remains fully `tr()`-based (`en` / `pt-BR`).

## Consequences

- Future LLM backends may only emit `AiPlan` JSON that still passes validation.  
- Unsupported prompts fail closed with a clear error.  
- Placement uses optional `tx/ty/tz` feature parameters (mm).
