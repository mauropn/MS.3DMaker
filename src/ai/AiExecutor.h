#pragma once

#include "ai/AiTypes.h"
#include "features/FeatureHistory.h"

namespace ms3d::ai {

/// Execute a validated plan exclusively through FeatureHistory APIs (no arbitrary memory writes).
AiExecutionResult executePlan(ms3d::features::FeatureHistory& history, const AiPlan& plan);

} // namespace ms3d::ai
