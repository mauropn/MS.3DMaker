#include "Sketch.h"

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Builder.hxx>
#include <Precision.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <algorithm>
#include <cmath>

namespace ms3d::sketch {
namespace {

gp_Ax2 planeAx2(PlaneId plane) {
    switch (plane) {
    case PlaneId::XZ:
        return gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0), gp_Dir(1, 0, 0));
    case PlaneId::YZ:
        return gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0));
    case PlaneId::XY:
    default:
        return gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1), gp_Dir(1, 0, 0));
    }
}

gp_Pnt to3d(PlaneId plane, const Vec2& uv) {
    const gp_Ax2 ax = planeAx2(plane);
    const gp_Vec x(ax.XDirection());
    const gp_Vec y(ax.YDirection());
    return ax.Location().Translated(x * uv.u + y * uv.v);
}

bool addLineEdge(TopoDS_Compound& compound, PlaneId plane, const Vec2& a, const Vec2& b) {
    if (std::hypot(a.u - b.u, a.v - b.v) < Precision::Confusion()) {
        return false;
    }
    BRep_Builder builder;
    builder.Add(compound, BRepBuilderAPI_MakeEdge(to3d(plane, a), to3d(plane, b)).Edge());
    return true;
}

} // namespace

const char* planeIdName(PlaneId id) {
    switch (id) {
    case PlaneId::XY:
        return "XY";
    case PlaneId::XZ:
        return "XZ";
    case PlaneId::YZ:
        return "YZ";
    }
    return "XY";
}

bool planeIdFromName(const std::string& name, PlaneId& out) {
    if (name == "XY") {
        out = PlaneId::XY;
        return true;
    }
    if (name == "XZ") {
        out = PlaneId::XZ;
        return true;
    }
    if (name == "YZ") {
        out = PlaneId::YZ;
        return true;
    }
    return false;
}

const char* entityKindName(EntityKind kind) {
    switch (kind) {
    case EntityKind::Line:
        return "Line";
    case EntityKind::Circle:
        return "Circle";
    case EntityKind::Rectangle:
        return "Rectangle";
    }
    return "Line";
}

bool entityKindFromName(const std::string& name, EntityKind& out) {
    if (name == "Line") {
        out = EntityKind::Line;
        return true;
    }
    if (name == "Circle") {
        out = EntityKind::Circle;
        return true;
    }
    if (name == "Rectangle") {
        out = EntityKind::Rectangle;
        return true;
    }
    return false;
}

const char* constraintKindName(ConstraintKind kind) {
    switch (kind) {
    case ConstraintKind::Horizontal:
        return "Horizontal";
    case ConstraintKind::Vertical:
        return "Vertical";
    case ConstraintKind::FixedLength:
        return "FixedLength";
    case ConstraintKind::Coincident:
        return "Coincident";
    }
    return "Horizontal";
}

bool constraintKindFromName(const std::string& name, ConstraintKind& out) {
    if (name == "Horizontal") {
        out = ConstraintKind::Horizontal;
        return true;
    }
    if (name == "Vertical") {
        out = ConstraintKind::Vertical;
        return true;
    }
    if (name == "FixedLength") {
        out = ConstraintKind::FixedLength;
        return true;
    }
    if (name == "Coincident") {
        out = ConstraintKind::Coincident;
        return true;
    }
    return false;
}

const char* dimensionKindName(DimensionKind kind) {
    switch (kind) {
    case DimensionKind::Length:
        return "Length";
    case DimensionKind::Radius:
        return "Radius";
    }
    return "Length";
}

bool dimensionKindFromName(const std::string& name, DimensionKind& out) {
    if (name == "Length") {
        out = DimensionKind::Length;
        return true;
    }
    if (name == "Radius") {
        out = DimensionKind::Radius;
        return true;
    }
    return false;
}

Sketch::Sketch(PlaneId plane) : m_plane(plane) {
}

void Sketch::setPlane(PlaneId plane) {
    m_plane = plane;
}

void Sketch::clear() {
    m_entities.clear();
    m_constraints.clear();
    m_dimensions.clear();
    m_nextId = 1;
}

void Sketch::replaceContents(PlaneId plane, std::vector<SketchEntity> entities,
                             std::vector<SketchConstraint> constraints,
                             std::vector<SketchDimension> dimensions, int nextId) {
    m_plane = plane;
    m_entities = std::move(entities);
    m_constraints = std::move(constraints);
    m_dimensions = std::move(dimensions);
    m_nextId = nextId > 0 ? nextId : 1;
}

int Sketch::nextId() {
    return m_nextId++;
}

SketchEntity* Sketch::mutableEntity(int id) {
    for (auto& e : m_entities) {
        if (e.id == id) {
            return &e;
        }
    }
    return nullptr;
}

const SketchEntity* Sketch::entityPtr(int id) const {
    for (const auto& e : m_entities) {
        if (e.id == id) {
            return &e;
        }
    }
    return nullptr;
}

std::optional<SketchEntity> Sketch::findEntity(int id) const {
    if (const auto* e = entityPtr(id)) {
        return *e;
    }
    return std::nullopt;
}

double Sketch::lineLength(const SketchEntity& e) {
    return std::hypot(e.p1.u - e.p0.u, e.p1.v - e.p0.v);
}

int Sketch::addLine(const Vec2& a, const Vec2& b) {
    SketchEntity e;
    e.id = nextId();
    e.kind = EntityKind::Line;
    e.p0 = a;
    e.p1 = b;
    m_entities.push_back(e);

    SketchDimension dim;
    dim.id = nextId();
    dim.kind = DimensionKind::Length;
    dim.entityId = e.id;
    dim.value = lineLength(e);
    m_dimensions.push_back(dim);
    return e.id;
}

int Sketch::addCircle(const Vec2& center, double radius) {
    SketchEntity e;
    e.id = nextId();
    e.kind = EntityKind::Circle;
    e.p0 = center;
    e.r = radius;
    m_entities.push_back(e);

    SketchDimension dim;
    dim.id = nextId();
    dim.kind = DimensionKind::Radius;
    dim.entityId = e.id;
    dim.value = radius;
    m_dimensions.push_back(dim);
    return e.id;
}

int Sketch::addRectangle(const Vec2& corner, double width, double height) {
    SketchEntity e;
    e.id = nextId();
    e.kind = EntityKind::Rectangle;
    e.p0 = corner;
    e.w = width;
    e.h = height;
    m_entities.push_back(e);

    SketchDimension dimW;
    dimW.id = nextId();
    dimW.kind = DimensionKind::Length;
    dimW.entityId = e.id;
    dimW.value = std::abs(width);
    m_dimensions.push_back(dimW);
    return e.id;
}

bool Sketch::removeEntity(int id) {
    const auto it = std::remove_if(m_entities.begin(), m_entities.end(),
                                   [id](const SketchEntity& e) { return e.id == id; });
    if (it == m_entities.end()) {
        return false;
    }
    m_entities.erase(it, m_entities.end());
    m_constraints.erase(std::remove_if(m_constraints.begin(), m_constraints.end(),
                                       [id](const SketchConstraint& c) {
                                           return c.entityA == id || c.entityB == id;
                                       }),
                        m_constraints.end());
    m_dimensions.erase(std::remove_if(m_dimensions.begin(), m_dimensions.end(),
                                      [id](const SketchDimension& d) { return d.entityId == id; }),
                       m_dimensions.end());
    return true;
}

int Sketch::addConstraint(ConstraintKind kind, int entityA, int entityB, double value) {
    SketchConstraint c;
    c.id = nextId();
    c.kind = kind;
    c.entityA = entityA;
    c.entityB = entityB;
    c.value = value;
    m_constraints.push_back(c);
    return c.id;
}

int Sketch::addDimension(DimensionKind kind, int entityId, double value) {
    SketchDimension d;
    d.id = nextId();
    d.kind = kind;
    d.entityId = entityId;
    d.value = value;
    m_dimensions.push_back(d);
    return d.id;
}

void Sketch::applyOne(const SketchConstraint& c) {
    auto* a = mutableEntity(c.entityA);
    if (!a) {
        return;
    }

    switch (c.kind) {
    case ConstraintKind::Horizontal:
        if (a->kind == EntityKind::Line) {
            a->p1.v = a->p0.v;
        }
        break;
    case ConstraintKind::Vertical:
        if (a->kind == EntityKind::Line) {
            a->p1.u = a->p0.u;
        }
        break;
    case ConstraintKind::FixedLength:
        if (a->kind == EntityKind::Line) {
            const double len = lineLength(*a);
            if (len < Precision::Confusion() || c.value <= 0.0) {
                break;
            }
            const double s = c.value / len;
            a->p1.u = a->p0.u + (a->p1.u - a->p0.u) * s;
            a->p1.v = a->p0.v + (a->p1.v - a->p0.v) * s;
        } else if (a->kind == EntityKind::Circle && c.value > 0.0) {
            a->r = c.value;
        }
        break;
    case ConstraintKind::Coincident: {
        auto* b = mutableEntity(c.entityB);
        if (!a || !b || a->kind != EntityKind::Line || b->kind != EntityKind::Line) {
            break;
        }
        // value: 0 = A.p0 to B.p0, 1 = A.p1 to B.p0, 2 = A.p0 to B.p1, 3 = A.p1 to B.p1
        const int mode = static_cast<int>(c.value);
        Vec2* pa = (mode == 0 || mode == 2) ? &a->p0 : &a->p1;
        Vec2* pb = (mode == 0 || mode == 1) ? &b->p0 : &b->p1;
        *pb = *pa;
        break;
    }
    }

    // Refresh auto dimensions for the entity
    for (auto& d : m_dimensions) {
        if (d.entityId != a->id) {
            continue;
        }
        if (d.kind == DimensionKind::Length && a->kind == EntityKind::Line) {
            d.value = lineLength(*a);
        } else if (d.kind == DimensionKind::Radius && a->kind == EntityKind::Circle) {
            d.value = a->r;
        } else if (d.kind == DimensionKind::Length && a->kind == EntityKind::Rectangle) {
            d.value = std::abs(a->w);
        }
    }
}

void Sketch::applyConstraints() {
    for (const auto& c : m_constraints) {
        applyOne(c);
    }
}

ValidationResult Sketch::validate() const {
    ValidationResult result;
    for (const auto& e : m_entities) {
        if (e.kind == EntityKind::Line) {
            if (lineLength(e) < Precision::Confusion()) {
                result.ok = false;
                result.issues.push_back({"zero_length_line", "Line has zero length"});
            }
        } else if (e.kind == EntityKind::Circle) {
            if (e.r <= Precision::Confusion()) {
                result.ok = false;
                result.issues.push_back({"invalid_radius", "Circle radius must be positive"});
            }
        } else if (e.kind == EntityKind::Rectangle) {
            if (std::abs(e.w) <= Precision::Confusion() ||
                std::abs(e.h) <= Precision::Confusion()) {
                result.ok = false;
                result.issues.push_back(
                    {"invalid_rectangle", "Rectangle width and height must be non-zero"});
            }
        }
    }

    for (const auto& c : m_constraints) {
        if (!entityPtr(c.entityA)) {
            result.ok = false;
            result.issues.push_back(
                {"dangling_constraint", "Constraint references missing entity A"});
            continue;
        }
        if ((c.kind == ConstraintKind::Coincident) && !entityPtr(c.entityB)) {
            result.ok = false;
            result.issues.push_back(
                {"dangling_constraint", "Constraint references missing entity B"});
        }
        if (c.kind == ConstraintKind::Horizontal || c.kind == ConstraintKind::Vertical ||
            c.kind == ConstraintKind::FixedLength) {
            const auto* e = entityPtr(c.entityA);
            if (!e) {
                continue;
            }
            if (c.kind == ConstraintKind::Horizontal && e->kind == EntityKind::Line) {
                if (std::abs(e->p0.v - e->p1.v) > 1e-6) {
                    result.ok = false;
                    result.issues.push_back(
                        {"unsatisfied_horizontal", "Horizontal constraint not satisfied"});
                }
            }
            if (c.kind == ConstraintKind::Vertical && e->kind == EntityKind::Line) {
                if (std::abs(e->p0.u - e->p1.u) > 1e-6) {
                    result.ok = false;
                    result.issues.push_back(
                        {"unsatisfied_vertical", "Vertical constraint not satisfied"});
                }
            }
            if (c.kind == ConstraintKind::FixedLength && e->kind == EntityKind::Line) {
                if (std::abs(lineLength(*e) - c.value) > 1e-4) {
                    result.ok = false;
                    result.issues.push_back(
                        {"unsatisfied_length", "Fixed length constraint not satisfied"});
                }
            }
            if (c.kind == ConstraintKind::FixedLength && e->kind == EntityKind::Circle) {
                if (std::abs(e->r - c.value) > 1e-4) {
                    result.ok = false;
                    result.issues.push_back(
                        {"unsatisfied_radius", "Fixed radius constraint not satisfied"});
                }
            }
        }
    }

    for (const auto& d : m_dimensions) {
        if (!entityPtr(d.entityId)) {
            result.ok = false;
            result.issues.push_back({"dangling_dimension", "Dimension references missing entity"});
        }
        if (d.value <= 0.0) {
            result.ok = false;
            result.issues.push_back({"non_positive_dimension", "Dimension value must be positive"});
        }
    }

    return result;
}

bool Sketch::buildDisplayCompound(TopoDS_Compound& out, std::string* errorOut) const {
    try {
        BRep_Builder builder;
        builder.MakeCompound(out);

        for (const auto& e : m_entities) {
            if (e.kind == EntityKind::Line) {
                if (!addLineEdge(out, m_plane, e.p0, e.p1) && errorOut) {
                    *errorOut = "Skipped degenerate line";
                }
            } else if (e.kind == EntityKind::Circle) {
                if (e.r <= Precision::Confusion()) {
                    continue;
                }
                const gp_Ax2 ax = planeAx2(m_plane);
                const gp_Pnt center = to3d(m_plane, e.p0);
                gp_Ax2 circAx(center, ax.Direction(), ax.XDirection());
                gp_Circ circ(circAx, e.r);
                builder.Add(out, BRepBuilderAPI_MakeEdge(circ).Edge());
            } else if (e.kind == EntityKind::Rectangle) {
                const Vec2 c0 = e.p0;
                const Vec2 c1{e.p0.u + e.w, e.p0.v};
                const Vec2 c2{e.p0.u + e.w, e.p0.v + e.h};
                const Vec2 c3{e.p0.u, e.p0.v + e.h};
                addLineEdge(out, m_plane, c0, c1);
                addLineEdge(out, m_plane, c1, c2);
                addLineEdge(out, m_plane, c2, c3);
                addLineEdge(out, m_plane, c3, c0);
            }
        }
        return true;
    } catch (const Standard_Failure& ex) {
        if (errorOut) {
            *errorOut =
                ex.GetMessageString() ? ex.GetMessageString() : "Failed to build sketch compound";
        }
        return false;
    } catch (...) {
        if (errorOut) {
            *errorOut = "Unknown error building sketch compound";
        }
        return false;
    }
}

} // namespace ms3d::sketch
