# Phase 5.2 — Milestone 3: Sketcher

## Goal

Interactive 2D sketch on a principal plane: lines, circles, rectangles, selection, dimensions, basic constraints, and validation.

## Delivered

| Piece | Location |
|-------|----------|
| Sketch model | `src/sketch/Sketch*.{h,cpp}` |
| Constraint-solver ADR | `docs/decisions/0003-sketch-constraint-solver.md` |
| Viewport sketch tools | `src/viewport/OcctView.*` |
| Sketch menu | `src/ui/MainWindow.*` |
| Tests | `tests/test_sketch.cpp` |

## Usage

1. **Sketch → Line / Circle / Rectangle** — two clicks on the active plane  
2. **Sketch → Plane XY / XZ / YZ** — choose sketch plane  
3. **Sketch → Select** — click an orange sketch entity  
4. **Make Horizontal / Vertical** — constrain the selected line  
5. **Validate Sketch** — report entity/constraint/dimension health  

Camera still uses Shift+LMB orbit, MMB/Alt+LMB pan, wheel zoom.

## Constraint strategy

Lightweight in-tree apply + validate (see ADR 0003). No external solver in this milestone.

## Out of scope

Feature history, document I/O, sketch→extrude pipeline UI, drag-solve DOF solver.
