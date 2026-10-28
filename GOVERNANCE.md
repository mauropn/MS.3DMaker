# Governance

## Summary

MS3DMaker uses **founder-led governance**. Contributions are welcome; major product and architecture decisions remain with the founder until a future, transparent evolution of this document.

## Roles

| Role | Who | Authority |
|------|-----|-----------|
| Founder / project lead | GitHub [@mauropn](https://github.com/mauropn) | Roadmap, architecture, licensing, releases, repository settings, final merge authority |
| Contributors | Anyone submitting issues, discussions, or pull requests | Propose changes; no merge rights by default |
| Maintainers | Appointed later by the founder | Review and merge within delegated scope (none appointed yet) |

Legal name, employer, and private contact details are intentionally omitted from this file. Prefer GitHub issues/discussions for project communication.

## Decision process

1. **Everyday changes** (docs, bug fixes, small features): pull request + review; founder (or a future maintainer) merges.
2. **Consequential changes** (architecture, public API, license, roadmap themes, dropping platforms): open an issue or ADR under `docs/decisions/`; founder approval required before merge.
3. **Releases:** founder approval required. No automated public release publishing without that approval.

## Architecture Decision Records

Significant decisions are recorded in `docs/decisions/`. Accepted ADRs guide implementation; Proposed ADRs await founder lock.

## Evolution

This governance model may change as the community grows. Changes to `GOVERNANCE.md` itself require founder approval and should be announced in the changelog / a GitHub discussion.
