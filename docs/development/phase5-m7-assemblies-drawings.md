# Phase 5.6 — Milestone 7: Assemblies and drawings

## Design principle

**Assemblies** and **drawings** are separate capabilities. Neither depends on the other. Both consume BREP shapes (tip feature, imported STEP, or assembly compound) and produce displayable OCCT shapes. They are not yet persisted inside `.ms3d` schema v1 (session tools; persistence is a later schema bump).

## Assemblies — requirements

| ID | Requirement | MVP |
|----|-------------|-----|
| A1 | Named component instances with rigid transforms (translation mm) | Yes |
| A2 | Add instance from current solid source | Yes |
| A3 | Build compound of transformed instances for display | Yes |
| A4 | Clear / list instance count | Yes |
| A5 | Mate constraints / joints | Deferred |
| A6 | Multi-document part references | Deferred |
| A7 | Persist assembly in `.ms3d` | Deferred (schema ≥2) |

## Drawings — requirements

| ID | Requirement | MVP |
|----|-------------|-----|
| D1 | Orthographic projections: Front / Top / Right | Yes |
| D2 | Isometric-style projection | Yes |
| D3 | Hidden-line removal via OCCT HLR → visible+smooth edges | Yes |
| D4 | Display projected edge compound in viewport | Yes |
| D5 | Dimensioned drawing sheets / title blocks | Deferred |
| D6 | DXF/PDF export of drawings | Deferred |
| D7 | Persist drawings in `.ms3d` | Deferred |

## Modules

- `src/assembly/Assembly.*` — instances + compound builder  
- `src/drawing/DrawingViews.*` — HLR projection helpers  
- UI: **Assembly** and **Drawing** menus  

## Tests

- Assembly: two translated boxes → compound with ≥2 solids  
- Drawing: box → Front HLR compound non-null with edges  
