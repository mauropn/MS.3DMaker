#pragma once

#include "features/FeatureTypes.h"

#include <string>
#include <vector>

namespace ms3d::ai {

/// One validated modeling step. Executors may only run these kinds via FeatureHistory.
enum class AiOpKind {
    MakeBox,
    MakeCylinder,
    MakeSphere,
    MakeExtrusion,
    MakeRevolution,
    BooleanUnion,
    BooleanCut,
    BooleanCommon
};

struct AiStep {
    int id = 0; // local plan id (1-based)
    AiOpKind op = AiOpKind::MakeBox;
    std::string label; // English technical summary for logs/preview
    std::vector<ms3d::features::FeatureParam> params;
    int inputA = -1; // plan step id for boolean A
    int inputB = -1; // plan step id for boolean B
};

struct AiPlan {
    std::string sourcePrompt;
    std::vector<AiStep> steps;
    std::vector<std::string> notes; // English technical notes / limitations
};

struct AiValidationIssue {
    std::string code;
    std::string message;
};

struct AiValidationResult {
    bool ok = true;
    std::vector<AiValidationIssue> issues;
};

struct AiExecutionResult {
    bool ok = false;
    std::string message;
    std::vector<int> createdFeatureIds; // FeatureHistory ids
};

const char* aiOpKindName(AiOpKind op);
bool aiOpKindFromName(const std::string& name, AiOpKind& out);
ms3d::features::FeatureKind toFeatureKind(AiOpKind op);
bool isBooleanOp(AiOpKind op);

} // namespace ms3d::ai
