# Phase 5.3 — Milestone 4: Feature history

## Goal

Parametric feature definitions with parameters, dependency graph, invalidation, recompute, edit-in-place, and error handling when a feature fails.

## Delivered

| Piece | Location |
|-------|----------|
| Feature model + history | `src/features/FeatureHistory.*`, `FeatureTypes.h` |
| History dock + edit dialog | `MainWindow`, `FeatureEditDialog` |
| Tests | `tests/test_feature_history.cpp` |

## Behaviour

- **Create** menu appends features to `FeatureHistory` (not one-off solids).
- Left dock lists features with status: OK / Dirty / Error.
- Select a feature to display its solid; double-click or **Edit → Edit Feature Parameters…** to change params.
- Parameter edits mark the feature and dependents dirty, then `recompute()` runs in topological order.
- Boolean features depend on two prior solids; upstream failure marks dependents invalid with an error message.

## Out of scope

Document save/load (Milestone 5), sketch→feature link, suppression/rollback UI beyond clear history.
