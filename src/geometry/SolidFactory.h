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

/// Axis-aligned box with dimensions in millimetres (origin corner).
SolidResult makeBox(double dx, double dy, double dz);

/// Cylinder along +Z with given radius and height (mm).
SolidResult makeCylinder(double radius, double height);

/// Sphere centred at the origin with given radius (mm).
SolidResult makeSphere(double radius);

/// Extrude a rectangular profile (width × depth on XY) by height along +Z.
SolidResult makeExtrusion(double width, double depth, double height);

/// Revolve a rectangular XZ profile (inner..outer radius, height) about Z → tube.
SolidResult makeRevolution(double innerRadius, double outerRadius, double height);

SolidResult booleanUnion(const TopoDS_Shape& a, const TopoDS_Shape& b);
SolidResult booleanCut(const TopoDS_Shape& a, const TopoDS_Shape& b);
SolidResult booleanCommon(const TopoDS_Shape& a, const TopoDS_Shape& b);

bool isValidSolid(const TopoDS_Shape& shape);

} // namespace ms3d::geometry
