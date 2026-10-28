#pragma once

#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>
#include <string>

namespace ms3d::geometry {

struct SolidResult {
    bool ok = false;
    TopoDS_Solid solid;
    std::string error; // English technical message for logs (not UI)
};

/// Create an axis-aligned box with the given dimensions (document units: mm).
SolidResult makeBox(double dx, double dy, double dz);

bool isValidSolid(const TopoDS_Shape& shape);

} // namespace ms3d::geometry
