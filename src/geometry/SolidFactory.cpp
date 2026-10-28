#include "SolidFactory.h"

#include <BRepAlgoAPI_BooleanOperation.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Standard_Failure.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

namespace ms3d::geometry {
namespace {

SolidResult finalizeFromMakerSolid(const TopoDS_Solid& solid, const char* nullMessage)
{
    SolidResult result;
    if (solid.IsNull()) {
        result.error = nullMessage ? nullMessage : "Null solid";
        return result;
    }
    if (!isValidSolid(solid)) {
        result.error = "Created solid failed validity check";
        return result;
    }
    result.ok = true;
    result.solid = solid;
    return result;
}

SolidResult runBoolean(BRepAlgoAPI_BooleanOperation& op, const char* failMessage)
{
    SolidResult result;
    try {
        op.Build();
        if (!op.IsDone()) {
            result.error = failMessage ? failMessage : "Boolean operation failed";
            return result;
        }
        const TopoDS_Shape& shape = op.Shape();
        if (shape.IsNull()) {
            result.error = "Boolean produced a null shape";
            return result;
        }

        if (shape.ShapeType() == TopAbs_SOLID) {
            return finalizeFromMakerSolid(TopoDS::Solid(shape), "Boolean produced a null solid");
        }

        // Fuse/Cut/Common often yield COMPOUND containing one solid
        TopExp_Explorer exp(shape, TopAbs_SOLID);
        if (!exp.More()) {
            result.error = "Boolean produced no solid";
            return result;
        }
        const TopoDS_Solid solid = TopoDS::Solid(exp.Current());
        exp.Next();
        if (exp.More()) {
            // Multiple solids — still accept first if overall shape is valid enough
            // Milestone 2: require a single solid for predictability
            result.error = "Boolean produced multiple solids";
            return result;
        }
        return finalizeFromMakerSolid(solid, "Boolean produced a null solid");
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure";
        return result;
    } catch (...) {
        result.error = "Unknown exception during boolean operation";
        return result;
    }
}

TopoDS_Face makeRectangleFaceXY(double width, double depth)
{
    const gp_Pnt p0(0.0, 0.0, 0.0);
    const gp_Pnt p1(width, 0.0, 0.0);
    const gp_Pnt p2(width, depth, 0.0);
    const gp_Pnt p3(0.0, depth, 0.0);

    BRepBuilderAPI_MakeWire wire;
    wire.Add(BRepBuilderAPI_MakeEdge(p0, p1).Edge());
    wire.Add(BRepBuilderAPI_MakeEdge(p1, p2).Edge());
    wire.Add(BRepBuilderAPI_MakeEdge(p2, p3).Edge());
    wire.Add(BRepBuilderAPI_MakeEdge(p3, p0).Edge());
    return BRepBuilderAPI_MakeFace(wire.Wire()).Face();
}

TopoDS_Face makeRectangleFaceXZ(double x0, double x1, double z0, double z1)
{
    const gp_Pnt p0(x0, 0.0, z0);
    const gp_Pnt p1(x1, 0.0, z0);
    const gp_Pnt p2(x1, 0.0, z1);
    const gp_Pnt p3(x0, 0.0, z1);

    BRepBuilderAPI_MakeWire wire;
    wire.Add(BRepBuilderAPI_MakeEdge(p0, p1).Edge());
    wire.Add(BRepBuilderAPI_MakeEdge(p1, p2).Edge());
    wire.Add(BRepBuilderAPI_MakeEdge(p2, p3).Edge());
    wire.Add(BRepBuilderAPI_MakeEdge(p3, p0).Edge());
    return BRepBuilderAPI_MakeFace(wire.Wire()).Face();
}

} // namespace

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
        return finalizeFromMakerSolid(maker.Solid(), "BRepPrimAPI_MakeBox produced a null solid");
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure";
        return result;
    } catch (...) {
        result.error = "Unknown exception while creating box";
        return result;
    }
}

SolidResult makeCylinder(double radius, double height)
{
    SolidResult result;
    if (radius <= 0.0 || height <= 0.0) {
        result.error = "Cylinder radius and height must be positive";
        return result;
    }

    try {
        BRepPrimAPI_MakeCylinder maker(radius, height);
        maker.Build();
        return finalizeFromMakerSolid(maker.Solid(),
                                      "BRepPrimAPI_MakeCylinder produced a null solid");
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure";
        return result;
    } catch (...) {
        result.error = "Unknown exception while creating cylinder";
        return result;
    }
}

SolidResult makeSphere(double radius)
{
    SolidResult result;
    if (radius <= 0.0) {
        result.error = "Sphere radius must be positive";
        return result;
    }

    try {
        BRepPrimAPI_MakeSphere maker(radius);
        maker.Build();
        return finalizeFromMakerSolid(maker.Solid(), "BRepPrimAPI_MakeSphere produced a null solid");
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure";
        return result;
    } catch (...) {
        result.error = "Unknown exception while creating sphere";
        return result;
    }
}

SolidResult makeExtrusion(double width, double depth, double height)
{
    SolidResult result;
    if (width <= 0.0 || depth <= 0.0 || height <= 0.0) {
        result.error = "Extrusion dimensions must be positive";
        return result;
    }

    try {
        const TopoDS_Face face = makeRectangleFaceXY(width, depth);
        if (face.IsNull()) {
            result.error = "Failed to build extrusion profile face";
            return result;
        }
        BRepPrimAPI_MakePrism prism(face, gp_Vec(0.0, 0.0, height));
        prism.Build();
        if (!prism.IsDone()) {
            result.error = "BRepPrimAPI_MakePrism failed";
            return result;
        }
        const TopoDS_Shape& shape = prism.Shape();
        if (shape.ShapeType() == TopAbs_SOLID) {
            return finalizeFromMakerSolid(TopoDS::Solid(shape), "Extrusion produced a null solid");
        }
        // Prism of a face typically yields a solid
        TopExp_Explorer exp(shape, TopAbs_SOLID);
        if (!exp.More()) {
            result.error = "Extrusion produced no solid";
            return result;
        }
        return finalizeFromMakerSolid(TopoDS::Solid(exp.Current()),
                                      "Extrusion produced a null solid");
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure";
        return result;
    } catch (...) {
        result.error = "Unknown exception while creating extrusion";
        return result;
    }
}

SolidResult makeRevolution(double innerRadius, double outerRadius, double height)
{
    SolidResult result;
    if (innerRadius < 0.0 || outerRadius <= 0.0 || height <= 0.0) {
        result.error = "Revolution radii and height must be valid (outer > 0, height > 0)";
        return result;
    }
    if (outerRadius <= innerRadius) {
        result.error = "Revolution outer radius must be greater than inner radius";
        return result;
    }

    try {
        const TopoDS_Face face = makeRectangleFaceXZ(innerRadius, outerRadius, 0.0, height);
        if (face.IsNull()) {
            result.error = "Failed to build revolution profile face";
            return result;
        }
        const gp_Ax1 axis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
        BRepPrimAPI_MakeRevol revol(face, axis);
        revol.Build();
        if (!revol.IsDone()) {
            result.error = "BRepPrimAPI_MakeRevol failed";
            return result;
        }
        const TopoDS_Shape& shape = revol.Shape();
        if (shape.ShapeType() == TopAbs_SOLID) {
            return finalizeFromMakerSolid(TopoDS::Solid(shape),
                                          "Revolution produced a null solid");
        }
        TopExp_Explorer exp(shape, TopAbs_SOLID);
        if (!exp.More()) {
            result.error = "Revolution produced no solid";
            return result;
        }
        return finalizeFromMakerSolid(TopoDS::Solid(exp.Current()),
                                      "Revolution produced a null solid");
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure";
        return result;
    } catch (...) {
        result.error = "Unknown exception while creating revolution";
        return result;
    }
}

SolidResult booleanUnion(const TopoDS_Shape& a, const TopoDS_Shape& b)
{
    SolidResult result;
    if (a.IsNull() || b.IsNull()) {
        result.error = "Boolean union requires two non-null shapes";
        return result;
    }
    BRepAlgoAPI_Fuse fuse(a, b);
    return runBoolean(fuse, "Boolean union failed");
}

SolidResult booleanCut(const TopoDS_Shape& a, const TopoDS_Shape& b)
{
    SolidResult result;
    if (a.IsNull() || b.IsNull()) {
        result.error = "Boolean cut requires two non-null shapes";
        return result;
    }
    BRepAlgoAPI_Cut cut(a, b);
    return runBoolean(cut, "Boolean cut failed");
}

SolidResult booleanCommon(const TopoDS_Shape& a, const TopoDS_Shape& b)
{
    SolidResult result;
    if (a.IsNull() || b.IsNull()) {
        result.error = "Boolean common requires two non-null shapes";
        return result;
    }
    BRepAlgoAPI_Common common(a, b);
    return runBoolean(common, "Boolean common failed");
}

} // namespace ms3d::geometry
