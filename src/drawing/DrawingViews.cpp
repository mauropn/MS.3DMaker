#include "DrawingViews.h"

#include <BRep_Builder.hxx>
#include <HLRAlgo_Projector.hxx>
#include <HLRBRep_Algo.hxx>
#include <HLRBRep_HLRToShape.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Iterator.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

namespace ms3d::drawing {

const char* viewOrientationName(ViewOrientation orientation) {
    switch (orientation) {
    case ViewOrientation::Front:
        return "Front";
    case ViewOrientation::Top:
        return "Top";
    case ViewOrientation::Right:
        return "Right";
    case ViewOrientation::Iso:
        return "Iso";
    }
    return "Front";
}

gp_Dir viewDirection(ViewOrientation orientation) {
    switch (orientation) {
    case ViewOrientation::Top:
        return gp_Dir(0, 0, -1);
    case ViewOrientation::Right:
        return gp_Dir(-1, 0, 0);
    case ViewOrientation::Iso:
        return gp_Dir(-1, -1, -1);
    case ViewOrientation::Front:
    default:
        return gp_Dir(0, -1, 0);
    }
}

DrawingViewResult makeHlrView(const TopoDS_Shape& shape, ViewOrientation orientation) {
    DrawingViewResult result;
    result.orientation = orientation;
    if (shape.IsNull()) {
        result.error = "Cannot create drawing view from a null shape";
        return result;
    }

    try {
        Handle(HLRBRep_Algo) algo = new HLRBRep_Algo();
        algo->Add(shape);

        const gp_Dir dir = viewDirection(orientation);
        gp_Ax2 ax(gp_Pnt(0, 0, 0), dir);
        HLRAlgo_Projector projector(ax);
        algo->Projector(projector);
        algo->Update();
        algo->Hide();

        HLRBRep_HLRToShape hlrToShape(algo);
        const TopoDS_Shape visible = hlrToShape.VCompound();
        const TopoDS_Shape smooth = hlrToShape.Rg1LineVCompound();
        const TopoDS_Shape outline = hlrToShape.OutLineVCompound();

        BRep_Builder builder;
        builder.MakeCompound(result.edges);
        auto addIf = [&](const TopoDS_Shape& s) {
            if (!s.IsNull()) {
                builder.Add(result.edges, s);
            }
        };
        addIf(visible);
        addIf(smooth);
        addIf(outline);

        // Empty compound is still a failure — projection produced nothing useful
        TopoDS_Iterator it(result.edges);
        if (!it.More()) {
            result.error = "HLR projection produced no visible edges";
            return result;
        }

        result.ok = true;
        return result;
    } catch (const Standard_Failure& ex) {
        result.error =
            ex.GetMessageString() ? ex.GetMessageString() : "OCCT failure during HLR projection";
        return result;
    } catch (...) {
        result.error = "Unknown exception during HLR projection";
        return result;
    }
}

} // namespace ms3d::drawing
