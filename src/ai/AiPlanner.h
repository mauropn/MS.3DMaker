#pragma once

#include "ai/AiTypes.h"

#include <string>

namespace ms3d::ai {

struct PlanResult {
    bool ok = false;
    AiPlan plan;
    std::string error; // English technical
};

/// Constrained planner: maps supported natural-language / recipe phrases to AiPlan.
/// Does not call external LLMs or execute code — only emits structured steps.
PlanResult planFromPrompt(const std::string& prompt);

} // namespace ms3d::ai
