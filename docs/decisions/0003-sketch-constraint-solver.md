# ADR 0003 — Sketch constraint solver strategy

**Status:** Accepted  
**Date:** 2026-10-28  
**Deciders:** Project founder ([mauropn](https://github.com/mauropn))  

## Problem

Milestone 3 needs basic geometric constraints and sketch validation. Full parametric sketchers usually depend on a dedicated 2D constraint solver. Choosing and integrating one too early risks license friction, large dependencies, and delayed delivery of lines/circles/planes.

## Decision

For Milestone 3:

1. Ship an **in-tree lightweight constraint layer** that applies a small set of constraints by direct geometry updates (horizontal, vertical, fixed length / radius) plus validation reports.
2. **Do not** vendor SolveSpace, FreeCAD PlaneGCS, or another full solver yet.
3. Re-evaluate a full solver when under-constrained multi-entity sketches and drag-solve become product requirements (likely with Feature history / sketcher polish).

## Alternatives considered

| Option | Notes | Outcome |
|--------|-------|---------|
| SolveSpace (slvs) | Capable; separate C API; license review needed | Deferred |
| FreeCAD PlaneGCS | Proven in CAD; GPL coupling risk for dual MIT/Apache app | Rejected for core |
| Homemade DOF solver | Large R&D; easy to get wrong | Deferred |
| Lightweight apply + validate | Matches Milestone 3 scope; no new deps | **Chosen** |

## Consequences

- Constraints are explicit and testable; advanced drag-solving is out of scope for M3.
- A future ADR can adopt an external solver without rewriting the sketch entity model if entity IDs and constraint kinds stay stable.
- All user-facing sketcher strings remain in `en` / `pt-BR` catalogs.
