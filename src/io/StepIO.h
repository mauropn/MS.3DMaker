#pragma once

#include <TopoDS_Shape.hxx>

#include <string>

namespace ms3d::io {

struct StepResult {
    bool ok = false;
    TopoDS_Shape shape;
    std::string error;   // English technical
    std::string warning; // non-fatal notes
    int transferredRoots = 0;
};

enum class StepSchema {
    AP214, // default automotive/design (ISO 10303-214)
    AP203, // configuration controlled design
    AP242  // managed model based 3D engineering (when supported by OCCT build)
};

const char* stepSchemaName(StepSchema schema);

/// Read a STEP file (.stp / .step) into an OCCT shape. Validates solids when present.
StepResult importStep(const std::string& filePath);

/// Write a shape to STEP using the given schema (default AP214).
StepResult exportStep(const TopoDS_Shape& shape, const std::string& filePath,
                      StepSchema schema = StepSchema::AP214);

/// Export then re-import; checks that the reloaded shape is non-null and BRep-valid when solid.
StepResult roundTripStep(const TopoDS_Shape& shape, const std::string& tempFilePath,
                         StepSchema schema = StepSchema::AP214);

} // namespace ms3d::io
