# Phase 5.1 — Milestone 2: Basic geometry

## Goal

Expose a tested geometry factory for primitives, built-in extrude/revolve profiles, and boolean ops, with Create-menu demos in the OCCT viewport.

## Delivered

| Piece | Location |
|-------|----------|
| Geometry API | `src/geometry/SolidFactory.{h,cpp}` |
| Viewport display | `OcctView::displaySolid` |
| Create menu demos | `src/ui/MainWindow.{h,cpp}` |
| Unit tests | `tests/test_geometry.cpp` |

## Operations (document units: mm)

| Op | API | Demo parameters |
|----|-----|-----------------|
| Box | `makeBox` | 100 × 60 × 30 |
| Cylinder | `makeCylinder` | r=25, h=60 |
| Sphere | `makeSphere` | r=40 |
| Extrusion | `makeExtrusion` | 80 × 50 profile × 20 height (XY rectangle + prism) |
| Revolution | `makeRevolution` | r_in=15, r_out=30, h=50 (XZ profile about Z) |
| Union / Cut / Common | `booleanUnion` / `booleanCut` / `booleanCommon` | two overlapping boxes |

Extrude/revolve use **built-in rectangular profiles**, not a sketcher (Milestone 3).

## Out of scope

Sketcher, feature history, document I/O, STEP, assemblies, AI.
