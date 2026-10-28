#include "AiTypes.h"

namespace ms3d::ai {

const char* aiOpKindName(AiOpKind op)
{
    switch (op) {
    case AiOpKind::MakeBox:
        return "MakeBox";
    case AiOpKind::MakeCylinder:
        return "MakeCylinder";
    case AiOpKind::MakeSphere:
        return "MakeSphere";
    case AiOpKind::MakeExtrusion:
        return "MakeExtrusion";
    case AiOpKind::MakeRevolution:
        return "MakeRevolution";
    case AiOpKind::BooleanUnion:
        return "BooleanUnion";
    case AiOpKind::BooleanCut:
        return "BooleanCut";
    case AiOpKind::BooleanCommon:
        return "BooleanCommon";
    }
    return "MakeBox";
}

bool aiOpKindFromName(const std::string& name, AiOpKind& out)
{
    if (name == "MakeBox") {
        out = AiOpKind::MakeBox;
        return true;
    }
    if (name == "MakeCylinder") {
        out = AiOpKind::MakeCylinder;
        return true;
    }
    if (name == "MakeSphere") {
        out = AiOpKind::MakeSphere;
        return true;
    }
    if (name == "MakeExtrusion") {
        out = AiOpKind::MakeExtrusion;
        return true;
    }
    if (name == "MakeRevolution") {
        out = AiOpKind::MakeRevolution;
        return true;
    }
    if (name == "BooleanUnion") {
        out = AiOpKind::BooleanUnion;
        return true;
    }
    if (name == "BooleanCut") {
        out = AiOpKind::BooleanCut;
        return true;
    }
    if (name == "BooleanCommon") {
        out = AiOpKind::BooleanCommon;
        return true;
    }
    return false;
}

ms3d::features::FeatureKind toFeatureKind(AiOpKind op)
{
    switch (op) {
    case AiOpKind::MakeBox:
        return ms3d::features::FeatureKind::Box;
    case AiOpKind::MakeCylinder:
        return ms3d::features::FeatureKind::Cylinder;
    case AiOpKind::MakeSphere:
        return ms3d::features::FeatureKind::Sphere;
    case AiOpKind::MakeExtrusion:
        return ms3d::features::FeatureKind::Extrusion;
    case AiOpKind::MakeRevolution:
        return ms3d::features::FeatureKind::Revolution;
    case AiOpKind::BooleanUnion:
        return ms3d::features::FeatureKind::BooleanUnion;
    case AiOpKind::BooleanCut:
        return ms3d::features::FeatureKind::BooleanCut;
    case AiOpKind::BooleanCommon:
        return ms3d::features::FeatureKind::BooleanCommon;
    }
    return ms3d::features::FeatureKind::Box;
}

bool isBooleanOp(AiOpKind op)
{
    return op == AiOpKind::BooleanUnion || op == AiOpKind::BooleanCut
           || op == AiOpKind::BooleanCommon;
}

} // namespace ms3d::ai
