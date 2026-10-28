#pragma once

#include "features/FeatureTypes.h"
#include "geometry/SolidFactory.h"

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace ms3d::features {

/// Ordered feature graph with parameters, dependencies, and recompute.
class FeatureHistory {
public:
    const std::vector<Feature>& features() const { return m_features; }

    int addPrimitive(FeatureKind kind, const std::vector<FeatureParam>& params,
                     const std::string& namePrefix);
    int addBoolean(FeatureKind kind, int featureA, int featureB, const std::string& namePrefix);

    std::optional<Feature> find(int id) const;
    Feature* findMutable(int id);

    bool setParameter(int id, const std::string& key, double value, std::string* errorOut = nullptr);
    bool setParameters(int id, const std::vector<FeatureParam>& params,
                       std::string* errorOut = nullptr);

    void invalidate(int id);
    RecomputeReport recompute();

    std::optional<Feature> tip() const;

    std::vector<int> directDependents(int id) const;
    std::vector<int> allDependents(int id) const;

    bool hasCycle() const;
    void clear();

private:
    int nextId();
    static bool isBooleanKind(FeatureKind kind);
    ms3d::geometry::SolidResult evaluate(const Feature& feature) const;
    std::vector<int> topologicalOrder() const;
    void collectDependents(int id, std::unordered_set<int>& out) const;
    double paramOr(const Feature& feature, const std::string& key, double fallback) const;
    const Feature* featurePtr(int id) const;

    std::vector<Feature> m_features;
    int m_nextId = 1;
    int m_nameSerial = 1;
};

} // namespace ms3d::features
