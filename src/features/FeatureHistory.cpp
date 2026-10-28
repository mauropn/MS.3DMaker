#include "FeatureHistory.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <algorithm>
#include <queue>
#include <sstream>
#include <unordered_map>

namespace ms3d::features {

const char* featureKindName(FeatureKind kind) {
    switch (kind) {
    case FeatureKind::Box:
        return "Box";
    case FeatureKind::Cylinder:
        return "Cylinder";
    case FeatureKind::Sphere:
        return "Sphere";
    case FeatureKind::Extrusion:
        return "Extrusion";
    case FeatureKind::Revolution:
        return "Revolution";
    case FeatureKind::BooleanUnion:
        return "BooleanUnion";
    case FeatureKind::BooleanCut:
        return "BooleanCut";
    case FeatureKind::BooleanCommon:
        return "BooleanCommon";
    }
    return "Feature";
}

bool featureKindFromName(const std::string& name, FeatureKind& out) {
    if (name == "Box") {
        out = FeatureKind::Box;
        return true;
    }
    if (name == "Cylinder") {
        out = FeatureKind::Cylinder;
        return true;
    }
    if (name == "Sphere") {
        out = FeatureKind::Sphere;
        return true;
    }
    if (name == "Extrusion") {
        out = FeatureKind::Extrusion;
        return true;
    }
    if (name == "Revolution") {
        out = FeatureKind::Revolution;
        return true;
    }
    if (name == "BooleanUnion") {
        out = FeatureKind::BooleanUnion;
        return true;
    }
    if (name == "BooleanCut") {
        out = FeatureKind::BooleanCut;
        return true;
    }
    if (name == "BooleanCommon") {
        out = FeatureKind::BooleanCommon;
        return true;
    }
    return false;
}

bool FeatureHistory::isBooleanKind(FeatureKind kind) {
    return kind == FeatureKind::BooleanUnion || kind == FeatureKind::BooleanCut ||
           kind == FeatureKind::BooleanCommon;
}

int FeatureHistory::nextId() {
    return m_nextId++;
}

void FeatureHistory::clear() {
    m_features.clear();
    m_nextId = 1;
    m_nameSerial = 1;
}

void FeatureHistory::replaceContents(std::vector<Feature> features, int nextId, int nameSerial) {
    m_features = std::move(features);
    for (auto& f : m_features) {
        f.dirty = true;
        f.valid = false;
        f.error.clear();
        f.result = TopoDS_Solid();
    }
    m_nextId = nextId > 0 ? nextId : 1;
    m_nameSerial = nameSerial > 0 ? nameSerial : 1;
}

const Feature* FeatureHistory::featurePtr(int id) const {
    for (const auto& f : m_features) {
        if (f.id == id) {
            return &f;
        }
    }
    return nullptr;
}

Feature* FeatureHistory::findMutable(int id) {
    for (auto& f : m_features) {
        if (f.id == id) {
            return &f;
        }
    }
    return nullptr;
}

std::optional<Feature> FeatureHistory::find(int id) const {
    if (const auto* f = featurePtr(id)) {
        return *f;
    }
    return std::nullopt;
}

std::optional<Feature> FeatureHistory::tip() const {
    if (m_features.empty()) {
        return std::nullopt;
    }
    return m_features.back();
}

double FeatureHistory::paramOr(const Feature& feature, const std::string& key,
                               double fallback) const {
    for (const auto& p : feature.params) {
        if (p.key == key) {
            return p.value;
        }
    }
    return fallback;
}

int FeatureHistory::addPrimitive(FeatureKind kind, const std::vector<FeatureParam>& params,
                                 const std::string& namePrefix) {
    if (isBooleanKind(kind)) {
        return -1;
    }
    Feature f;
    f.id = nextId();
    f.kind = kind;
    f.params = params;
    f.dirty = true;
    std::ostringstream name;
    name << (namePrefix.empty() ? featureKindName(kind) : namePrefix) << m_nameSerial++;
    f.name = name.str();
    m_features.push_back(f);
    return f.id;
}

int FeatureHistory::addBoolean(FeatureKind kind, int featureA, int featureB,
                               const std::string& namePrefix) {
    if (!isBooleanKind(kind)) {
        return -1;
    }
    if (!featurePtr(featureA) || !featurePtr(featureB)) {
        return -1;
    }
    Feature f;
    f.id = nextId();
    f.kind = kind;
    f.dependencyIds = {featureA, featureB};
    f.dirty = true;
    std::ostringstream name;
    name << (namePrefix.empty() ? featureKindName(kind) : namePrefix) << m_nameSerial++;
    f.name = name.str();
    m_features.push_back(f);
    return f.id;
}

std::vector<int> FeatureHistory::directDependents(int id) const {
    std::vector<int> out;
    for (const auto& f : m_features) {
        if (std::find(f.dependencyIds.begin(), f.dependencyIds.end(), id) !=
            f.dependencyIds.end()) {
            out.push_back(f.id);
        }
    }
    return out;
}

void FeatureHistory::collectDependents(int id, std::unordered_set<int>& out) const {
    for (int dep : directDependents(id)) {
        if (out.insert(dep).second) {
            collectDependents(dep, out);
        }
    }
}

std::vector<int> FeatureHistory::allDependents(int id) const {
    std::unordered_set<int> set;
    collectDependents(id, set);
    return std::vector<int>(set.begin(), set.end());
}

void FeatureHistory::invalidate(int id) {
    auto* f = findMutable(id);
    if (!f) {
        return;
    }
    f->dirty = true;
    f->valid = false;
    for (int dep : allDependents(id)) {
        if (auto* d = findMutable(dep)) {
            d->dirty = true;
            d->valid = false;
        }
    }
}

bool FeatureHistory::setParameter(int id, const std::string& key, double value,
                                  std::string* errorOut) {
    auto* f = findMutable(id);
    if (!f) {
        if (errorOut) {
            *errorOut = "Feature not found";
        }
        return false;
    }
    if (isBooleanKind(f->kind)) {
        if (errorOut) {
            *errorOut = "Boolean features have no numeric parameters";
        }
        return false;
    }
    bool found = false;
    for (auto& p : f->params) {
        if (p.key == key) {
            p.value = value;
            found = true;
            break;
        }
    }
    if (!found) {
        f->params.push_back({key, value});
    }
    invalidate(id);
    return true;
}

bool FeatureHistory::setParameters(int id, const std::vector<FeatureParam>& params,
                                   std::string* errorOut) {
    auto* f = findMutable(id);
    if (!f) {
        if (errorOut) {
            *errorOut = "Feature not found";
        }
        return false;
    }
    f->params = params;
    invalidate(id);
    return true;
}

ms3d::geometry::SolidResult FeatureHistory::evaluate(const Feature& feature) const {
    using ms3d::geometry::SolidResult;

    auto applyTranslation = [&](SolidResult created) -> SolidResult {
        if (!created.ok) {
            return created;
        }
        const double tx = paramOr(feature, "tx", 0.0);
        const double ty = paramOr(feature, "ty", 0.0);
        const double tz = paramOr(feature, "tz", 0.0);
        if (tx == 0.0 && ty == 0.0 && tz == 0.0) {
            return created;
        }
        try {
            gp_Trsf trsf;
            trsf.SetTranslation(gp_Vec(tx, ty, tz));
            BRepBuilderAPI_Transform xform(created.solid, trsf);
            created.solid = TopoDS::Solid(xform.Shape());
            return created;
        } catch (const Standard_Failure& ex) {
            created.ok = false;
            created.error = ex.GetMessageString() ? ex.GetMessageString()
                                                  : "Failed to apply feature translation";
            return created;
        }
    };

    switch (feature.kind) {
    case FeatureKind::Box:
        return applyTranslation(ms3d::geometry::makeBox(
            paramOr(feature, "dx", 0), paramOr(feature, "dy", 0), paramOr(feature, "dz", 0)));
    case FeatureKind::Cylinder:
        return applyTranslation(ms3d::geometry::makeCylinder(paramOr(feature, "radius", 0),
                                                             paramOr(feature, "height", 0)));
    case FeatureKind::Sphere:
        return applyTranslation(ms3d::geometry::makeSphere(paramOr(feature, "radius", 0)));
    case FeatureKind::Extrusion:
        return applyTranslation(ms3d::geometry::makeExtrusion(paramOr(feature, "width", 0),
                                                              paramOr(feature, "depth", 0),
                                                              paramOr(feature, "height", 0)));
    case FeatureKind::Revolution:
        return applyTranslation(ms3d::geometry::makeRevolution(paramOr(feature, "innerRadius", 0),
                                                               paramOr(feature, "outerRadius", 0),
                                                               paramOr(feature, "height", 0)));
    case FeatureKind::BooleanUnion:
    case FeatureKind::BooleanCut:
    case FeatureKind::BooleanCommon: {
        SolidResult fail;
        if (feature.dependencyIds.size() != 2) {
            fail.error = "Boolean feature requires two dependencies";
            return fail;
        }
        const auto* a = featurePtr(feature.dependencyIds[0]);
        const auto* b = featurePtr(feature.dependencyIds[1]);
        if (!a || !b) {
            fail.error = "Boolean dependency missing";
            return fail;
        }
        if (!a->valid || !b->valid) {
            fail.error = "Boolean dependency is invalid";
            return fail;
        }
        if (feature.kind == FeatureKind::BooleanUnion) {
            return ms3d::geometry::booleanUnion(a->result, b->result);
        }
        if (feature.kind == FeatureKind::BooleanCut) {
            return ms3d::geometry::booleanCut(a->result, b->result);
        }
        return ms3d::geometry::booleanCommon(a->result, b->result);
    }
    }
    SolidResult unknown;
    unknown.error = "Unknown feature kind";
    return unknown;
}

std::vector<int> FeatureHistory::topologicalOrder() const {
    std::unordered_map<int, int> indegree;
    std::unordered_map<int, std::vector<int>> adj;
    for (const auto& f : m_features) {
        indegree[f.id] = 0;
    }
    for (const auto& f : m_features) {
        for (int dep : f.dependencyIds) {
            adj[dep].push_back(f.id);
            indegree[f.id]++;
        }
    }
    std::queue<int> q;
    for (const auto& [id, deg] : indegree) {
        if (deg == 0) {
            q.push(id);
        }
    }
    std::vector<int> order;
    while (!q.empty()) {
        const int id = q.front();
        q.pop();
        order.push_back(id);
        for (int next : adj[id]) {
            if (--indegree[next] == 0) {
                q.push(next);
            }
        }
    }
    return order;
}

bool FeatureHistory::hasCycle() const {
    return topologicalOrder().size() != m_features.size();
}

RecomputeReport FeatureHistory::recompute() {
    RecomputeReport report;
    if (hasCycle()) {
        report.ok = false;
        report.message = "Feature dependency graph has a cycle";
        return report;
    }

    const auto order = topologicalOrder();
    for (int id : order) {
        auto* f = findMutable(id);
        if (!f || !f->dirty) {
            continue;
        }
        const auto result = evaluate(*f);
        if (!result.ok) {
            f->valid = false;
            f->dirty = false;
            f->error = result.error;
            f->result = TopoDS_Solid();
            report.ok = false;
            report.failedIds.push_back(id);
            report.message = f->name + ": " + result.error;
            // Dependents stay dirty / become invalid on next pass attempt
            for (int dep : allDependents(id)) {
                if (auto* d = findMutable(dep)) {
                    d->valid = false;
                    d->dirty = true;
                    d->error = "Upstream feature failed: " + f->name;
                }
            }
            continue;
        }
        f->result = result.solid;
        f->valid = true;
        f->dirty = false;
        f->error.clear();
        report.recomputedIds.push_back(id);
    }

    if (report.ok) {
        report.message = "Recompute succeeded";
    }
    return report;
}

} // namespace ms3d::features
