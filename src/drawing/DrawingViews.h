#pragma once

#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Dir.hxx>

#include <string>

namespace ms3d::drawing {

enum class ViewOrientation {
    Front, // looking along -Y (XY elevation)
    Top,   // looking along -Z
    Right, // looking along -X
    Iso    // isometric-ish direction
};

struct DrawingViewResult {
    bool ok = false;
    TopoDS_Compound edges; // visible + smooth projected edges in 3D (HLR output)
    std::string error;
    ViewOrientation orientation = ViewOrientation::Front;
};

const char* viewOrientationName(ViewOrientation orientation);

gp_Dir viewDirection(ViewOrientation orientation);

/// Build a hidden-line projection of `shape` for the given orientation (OCCT HLR).
DrawingViewResult makeHlrView(const TopoDS_Shape& shape, ViewOrientation orientation);

} // namespace ms3d::drawing
