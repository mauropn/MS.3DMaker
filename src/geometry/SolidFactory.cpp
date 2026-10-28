#include "SolidFactory.h"

#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>

namespace ms3d::geometry {

bool isValidSolid(const TopoDS_Shape& shape)
{
    if (shape.IsNull()) {
        return false;
    }
    BRepCheck_Analyzer analyzer(shape);
    return analyzer.IsValid();
}

SolidResult makeBox(double dx, double dy, double dz)
{
    SolidResult result;
    if (dx <= 0.0 || dy <= 0.0 || dz <= 0.0) {
        result.error = "Box dimensions must be positive";
        return result;
    }

    try {
        BRepPrimAPI_MakeBox maker(dx, dy, dz);
        maker.Build();
        const TopoDS_Solid solid = maker.Solid();
        if (solid.IsNull()) {
            result.error = "BRepPrimAPI_MakeBox produced a null solid";
            return result;
        }
        if (!isValidSolid(solid)) {
            result.error = "Created box solid failed validity check";
            return result;
        }
        result.ok = true;
        result.solid = solid;
        return result;
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure";
        return result;
    } catch (...) {
        result.error = "Unknown exception while creating box";
        return result;
    }
}

} // namespace ms3d::geometry
