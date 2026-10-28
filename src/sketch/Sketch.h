#pragma once

#include "sketch/SketchTypes.h"

#include <optional>
#include <string>

class TopoDS_Compound;

namespace ms3d::sketch {

/// Parametric-ish 2D sketch on a principal plane (document units: mm).
class Sketch {
public:
    explicit Sketch(PlaneId plane = PlaneId::XY);

    PlaneId plane() const { return m_plane; }
    void setPlane(PlaneId plane);

    const std::vector<SketchEntity>& entities() const { return m_entities; }
    const std::vector<SketchConstraint>& constraints() const { return m_constraints; }
    const std::vector<SketchDimension>& dimensions() const { return m_dimensions; }

    int addLine(const Vec2& a, const Vec2& b);
    int addCircle(const Vec2& center, double radius);
    int addRectangle(const Vec2& corner, double width, double height);

    bool removeEntity(int id);
    std::optional<SketchEntity> findEntity(int id) const;

    int addConstraint(ConstraintKind kind, int entityA, int entityB = -1, double value = 0.0);
    int addDimension(DimensionKind kind, int entityId, double value);

    /// Apply basic constraints by updating entity geometry in place.
    void applyConstraints();

    ValidationResult validate() const;

    /// Build OCCT edges for display (compound of edges on the sketch plane in 3D).
    bool buildDisplayCompound(TopoDS_Compound& out, std::string* errorOut = nullptr) const;

    void clear();

    void replaceContents(PlaneId plane, std::vector<SketchEntity> entities,
                         std::vector<SketchConstraint> constraints,
                         std::vector<SketchDimension> dimensions, int nextId);

    int peekNextId() const { return m_nextId; }

private:
    int nextId();
    SketchEntity* mutableEntity(int id);
    const SketchEntity* entityPtr(int id) const;
    static double lineLength(const SketchEntity& e);
    void applyOne(const SketchConstraint& c);

    PlaneId m_plane = PlaneId::XY;
    std::vector<SketchEntity> m_entities;
    std::vector<SketchConstraint> m_constraints;
    std::vector<SketchDimension> m_dimensions;
    int m_nextId = 1;
};

const char* planeIdName(PlaneId id);
bool planeIdFromName(const std::string& name, PlaneId& out);

const char* entityKindName(EntityKind kind);
bool entityKindFromName(const std::string& name, EntityKind& out);

const char* constraintKindName(ConstraintKind kind);
bool constraintKindFromName(const std::string& name, ConstraintKind& out);

const char* dimensionKindName(DimensionKind kind);
bool dimensionKindFromName(const std::string& name, DimensionKind& out);

} // namespace ms3d::sketch
