# Phase 5.5 — Milestone 6: CAD interchange (STEP)

## Goal

Import and export BREP geometry via STEP using OCCT `STEPControl_Reader` / `STEPControl_Writer`, with validity checks and a round-trip test.

## Delivered

| Piece | Location |
|-------|----------|
| STEP I/O API | `src/io/StepIO.{h,cpp}` |
| File menu | Import STEP… / Export STEP… |
| Tests | `tests/test_step.cpp` |

## Supported behaviour

- **Import** `.step` / `.stp` → validate with `BRepCheck_Analyzer` → display in viewport  
- **Export** tip feature solid (or last imported shape) as **AP214IS** (ISO 10303-214)  
- Units written as millimetres (`write.step.unit=MM`)  
- Linked OCCT kits: `TKDESTEP`, `TKXSBase`, `TKDE`

## Limitations

- STEP exchange is **geometry only** — not parametric feature history or sketches  
- Assemblies / PMI / colours are not preserved as first-class MS3DMaker features  
- AP203 / AP242 tokens are available in the API; UI export uses AP214IS  
- Invalid or empty STEP files are refused; import does not mutate `.ms3d` document schema  

## Round-trip

`roundTripStep` exports then re-imports; unit test covers a valid box solid through a temp `.step` file.
