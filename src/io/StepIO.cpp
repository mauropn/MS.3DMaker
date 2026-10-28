#include "StepIO.h"

#include "geometry/SolidFactory.h"

#include <BRepCheck_Analyzer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Controller.hxx>
#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <Standard_Failure.hxx>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Solid.hxx>

namespace ms3d::io {
namespace {

const char* occtSchemaToken(StepSchema schema)
{
    // Tokens registered by STEPControl_Controller (OCCT 7.x).
    switch (schema) {
    case StepSchema::AP203:
        return "AP203";
    case StepSchema::AP242:
        return "AP242DIS";
    case StepSchema::AP214:
    default:
        return "AP214IS";
    }
}

bool applyWriteSchema(StepSchema schema, std::string* errorOut)
{
    STEPControl_Controller::Init();
    const char* token = occtSchemaToken(schema);
    if (!Interface_Static::SetCVal("write.step.schema", token)) {
        // Fall back to whatever default the controller registered.
        if (errorOut) {
            *errorOut = std::string("Failed to set write.step.schema to ") + token
                        + "; continuing with OCCT default schema";
        }
        // Non-fatal: writer still works with default
    }
    Interface_Static::SetCVal("write.step.unit", "MM");
    return true;
}

bool shapeHasValidSolid(const TopoDS_Shape& shape)
{
    if (shape.IsNull()) {
        return false;
    }
    if (shape.ShapeType() == TopAbs_SOLID) {
        return ms3d::geometry::isValidSolid(TopoDS::Solid(shape));
    }
    for (TopExp_Explorer exp(shape, TopAbs_SOLID); exp.More(); exp.Next()) {
        if (!ms3d::geometry::isValidSolid(TopoDS::Solid(exp.Current()))) {
            return false;
        }
    }
    // Non-solid shapes: fall back to whole-shape check when it is a solid-like compound
    BRepCheck_Analyzer analyzer(shape);
    return analyzer.IsValid();
}

} // namespace

const char* stepSchemaName(StepSchema schema)
{
    switch (schema) {
    case StepSchema::AP203:
        return "AP203";
    case StepSchema::AP242:
        return "AP242";
    case StepSchema::AP214:
    default:
        return "AP214";
    }
}

StepResult importStep(const std::string& filePath)
{
    StepResult result;
    if (filePath.empty()) {
        result.error = "STEP import path is empty";
        return result;
    }

    try {
        STEPControl_Reader reader;
        const IFSelect_ReturnStatus status = reader.ReadFile(filePath.c_str());
        if (status != IFSelect_RetDone) {
            result.error = "STEP ReadFile failed (file missing, unreadable, or not STEP)";
            return result;
        }

        const Standard_Integer nRoots = reader.NbRootsForTransfer();
        if (nRoots <= 0) {
            result.error = "STEP file contains no transferable roots";
            return result;
        }

        reader.TransferRoots();
        result.transferredRoots = reader.NbShapes();
        result.shape = reader.OneShape();
        if (result.shape.IsNull()) {
            result.error = "STEP transfer produced a null shape";
            return result;
        }

        if (!shapeHasValidSolid(result.shape)) {
            result.warning = "Imported STEP shape failed BRep validity check";
            // Still return ok=false for invalid geometry so callers can refuse silently bad models
            result.error = result.warning;
            return result;
        }

        result.ok = true;
        return result;
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure on STEP import";
        return result;
    } catch (...) {
        result.error = "Unknown exception during STEP import";
        return result;
    }
}

StepResult exportStep(const TopoDS_Shape& shape, const std::string& filePath, StepSchema schema)
{
    StepResult result;
    result.shape = shape;
    if (shape.IsNull()) {
        result.error = "Cannot export a null shape to STEP";
        return result;
    }
    if (filePath.empty()) {
        result.error = "STEP export path is empty";
        return result;
    }

    try {
        std::string schemaError;
        if (!applyWriteSchema(schema, &schemaError)) {
            result.error = schemaError;
            return result;
        }

        STEPControl_Writer writer;
        const IFSelect_ReturnStatus tr = writer.Transfer(shape, STEPControl_AsIs);
        if (tr != IFSelect_RetDone) {
            result.error = "STEP Transfer failed";
            return result;
        }
        const IFSelect_ReturnStatus wr = writer.Write(filePath.c_str());
        if (wr != IFSelect_RetDone) {
            result.error = "STEP Write failed";
            return result;
        }
        result.ok = true;
        return result;
    } catch (const Standard_Failure& ex) {
        result.error = ex.GetMessageString() ? ex.GetMessageString() : "OCCT Standard_Failure on STEP export";
        return result;
    } catch (...) {
        result.error = "Unknown exception during STEP export";
        return result;
    }
}

StepResult roundTripStep(const TopoDS_Shape& shape, const std::string& tempFilePath, StepSchema schema)
{
    StepResult written = exportStep(shape, tempFilePath, schema);
    if (!written.ok) {
        return written;
    }
    StepResult loaded = importStep(tempFilePath);
    if (!loaded.ok) {
        loaded.error = "Round-trip re-import failed: " + loaded.error;
        return loaded;
    }
    loaded.warning = std::string("Round-trip OK via ") + stepSchemaName(schema);
    return loaded;
}

} // namespace ms3d::io
