#pragma once

#include <TopoDS_Solid.hxx>

#include <string>
#include <vector>

namespace ms3d::features {

enum class FeatureKind {
    Box,
    Cylinder,
    Sphere,
    Extrusion,
    Revolution,
    BooleanUnion,
    BooleanCut,
    BooleanCommon
};

struct FeatureParam {
    std::string key;
    double value = 0.0;
};

struct Feature {
    int id = 0;
    FeatureKind kind = FeatureKind::Box;
    std::string name;
    std::vector<FeatureParam> params;
    std::vector<int> dependencyIds;
    bool dirty = true;
    bool valid = false;
    std::string error; // English technical
    TopoDS_Solid result;
};

struct RecomputeReport {
    bool ok = true;
    std::vector<int> recomputedIds;
    std::vector<int> failedIds;
    std::string message;
};

const char* featureKindName(FeatureKind kind);
bool featureKindFromName(const std::string& name, FeatureKind& out);

} // namespace ms3d::features
