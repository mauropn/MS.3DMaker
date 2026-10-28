# Roadmap

Status after Phase 5 modeling milestones and Phase 6 community/CI.

## Done

| Phase | Topic |
|-------|--------|
| 0–1 | Toolchain, REA |
| 2 | i18n (`en` / `pt-BR`) |
| 3 | Governance + dual license |
| 4 | Viewport + first solid |
| 5.1–5.7 | Geometry → sketcher → features → documents → STEP → assemblies/drawings → AI Assist |

## Near-term (community-friendly)

Seeded starter issues (label `good first issue`):

1. [#1](https://github.com/mauropn/MS.3DMaker/issues/1) — geometry/feature invalid-param unit test  
2. [#2](https://github.com/mauropn/MS.3DMaker/issues/2) — viewport navigation shortcuts (i18n)  
3. [#3](https://github.com/mauropn/MS.3DMaker/issues/3) — one constrained AI Assist recipe + test  

Further ideas:

4. Sketch constraint UX polish (error messages, `tr()` coverage).  
5. Linux packaging notes (AppImage / Flatpak research — no binary publish until release process is approved).

## Explicitly deferred

- Automatic GitHub Releases / binary publishing (founder approval required — see [RELEASE.md](RELEASE.md)).  
- External LLM backends for AI Assist (must still emit validated `AiPlan` only).  
- Multi-maintainer governance changes.

## How to propose work

Open a feature request with the template. Keep PRs reviewable. Prefer issues linked from this roadmap over large speculative redesigns.
