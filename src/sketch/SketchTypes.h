#pragma once

#include <string>
#include <vector>

namespace ms3d::sketch {

enum class PlaneId { XY = 0, XZ = 1, YZ = 2 };

struct Vec2 {
    double u = 0.0;
    double v = 0.0;
};

enum class EntityKind { Line, Circle, Rectangle };

struct SketchEntity {
    int id = 0;
    EntityKind kind = EntityKind::Line;
    // Line: p0–p1; Circle: center=p0, radius=r; Rectangle: corner=p0, size=(w,h) along +u/+v
    Vec2 p0;
    Vec2 p1;
    double w = 0.0;
    double h = 0.0;
    double r = 0.0;
};

enum class ConstraintKind {
    Horizontal,   // line: endpoints share v
    Vertical,     // line: endpoints share u
    FixedLength,  // line length or circle radius
    Coincident    // line endpoint to another line endpoint (e1 endpoint index in value: 0/1)
};

struct SketchConstraint {
    int id = 0;
    ConstraintKind kind = ConstraintKind::Horizontal;
    int entityA = -1;
    int entityB = -1;
    double value = 0.0; // length/radius, or endpoint index for coincident
};

enum class DimensionKind { Length, Radius };

struct SketchDimension {
    int id = 0;
    DimensionKind kind = DimensionKind::Length;
    int entityId = -1;
    double value = 0.0;
};

struct ValidationIssue {
    std::string code;    // stable machine code
    std::string message; // English technical
};

struct ValidationResult {
    bool ok = true;
    std::vector<ValidationIssue> issues;
};

} // namespace ms3d::sketch
