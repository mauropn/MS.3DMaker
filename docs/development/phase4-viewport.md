# Phase 4 — OCCT viewport and first solid

## Goal

Ship a real 3D viewport in the main window: OCCT `V3d` / `AIS` behind a Qt widget, camera navigation, selection highlight, and a valid demo box solid.

## Delivered

| Piece | Location |
|-------|----------|
| `OcctView` Qt widget | `src/viewport/OcctView.{h,cpp}` |
| Box factory + validity | `src/geometry/SolidFactory.{h,cpp}` |
| Main window integration | `src/ui/MainWindow.{h,cpp}` |
| Geometry unit tests | `tests/test_geometry.cpp` |
| CMake OCCT link + DLL copy | root `CMakeLists.txt` |

## Viewport behavior

- Background + default lights via `V3d_Viewer`
- Corner triad (`TriedronDisplay`) and `AIS_ViewCube`
- **Orbit:** Shift + left-drag  
- **Pan:** middle-drag, or Alt + left-drag  
- **Zoom:** mouse wheel, or right-drag  
- **Select / highlight:** left-click (`AIS_InteractiveContext` MoveTo / Select)

## Geometry

- Document units: **millimetres**
- Demo solid: **100 × 60 × 30 mm** box (`File → New Box`, also on first show)
- Validity via `BRepCheck_Analyzer`

## Build notes

Requires `CASROOT` (or `C:/Dev/OCCT/install`) and Qt 6.8.3. On Windows, Release builds copy required OCCT DLLs next to `MS3DMaker.exe`.

```powershell
.\scripts\dev-shell.ps1
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=F:/Dev/Qt/6.8.3/msvc2022_64
cmake --build build
ctest --test-dir build --output-on-failure
.\build\MS3DMaker.exe
```
