# ADR 0002 — Project application license

**Status:** Accepted  
**Date:** 2026-10-28  
**Deciders:** Project founder ([mauropn](https://github.com/mauropn))  

## Problem

MS3DMaker needed a public application license compatible with Qt and OCCT (LGPL-family dependencies) and aligned with founder goals for an independent open-source CAD project.

## Decision

Adopt the same dual-license model as [SolidCraft](https://github.com/tangency-labs/solidcraft):

**MIT OR Apache-2.0, at the recipient’s option.**

Repository files:

- `LICENSE` — dual-license notice  
- `LICENSE-MIT` — MIT text  
- `LICENSE-APACHE` — Apache License 2.0 text  
- `NOTICE` — copyright and attribution pointer  
- `THIRD_PARTY_NOTICES.md` — Qt, OCCT, and other dependencies  

## Alternatives considered

| Option | Outcome |
|--------|---------|
| Apache-2.0 only | Strong default; superseded by dual license to match SolidCraft |
| MIT only | No express patent grant |
| LGPL-3.0 / GPL-3.0 | Rejected for the application (copyleft fit / contribution friction) |

## Consequences

- Recipients may use MS3DMaker under MIT or Apache-2.0.
- Contributions are accepted under the same dual terms (see `LICENSE` and `CONTRIBUTING.md`).
- Qt and OCCT obligations for **binary** distribution remain unchanged (LGPL-family compliance, notices).
- GitHub license metadata may show one SPDX id (often Apache-2.0); the dual choice is defined by `LICENSE`.

## Reference

SolidCraft README: “MIT OR Apache-2.0, at your option.”
