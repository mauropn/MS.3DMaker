#include "AiExecutor.h"
#include "AiValidator.h"

#include <unordered_map>

namespace ms3d::ai {

AiExecutionResult executePlan(ms3d::features::FeatureHistory& history, const AiPlan& plan)
{
    AiExecutionResult result;
    const auto validation = validatePlan(plan);
    if (!validation.ok) {
        result.message = validation.issues.empty() ? "Plan validation failed"
                                                   : validation.issues.front().message;
        return result;
    }

    std::unordered_map<int, int> planIdToFeatureId;

    for (const auto& step : plan.steps) {
        if (isBooleanOp(step.op)) {
            const auto itA = planIdToFeatureId.find(step.inputA);
            const auto itB = planIdToFeatureId.find(step.inputB);
            if (itA == planIdToFeatureId.end() || itB == planIdToFeatureId.end()) {
                result.message = "Boolean step missing resolved feature inputs";
                return result;
            }
            const int fid = history.addBoolean(toFeatureKind(step.op), itA->second, itB->second,
                                               aiOpKindName(step.op));
            if (fid < 0) {
                result.message = "FeatureHistory rejected boolean step";
                return result;
            }
            planIdToFeatureId[step.id] = fid;
            result.createdFeatureIds.push_back(fid);
        } else {
            const int fid =
                history.addPrimitive(toFeatureKind(step.op), step.params, aiOpKindName(step.op));
            if (fid < 0) {
                result.message = "FeatureHistory rejected primitive step";
                return result;
            }
            planIdToFeatureId[step.id] = fid;
            result.createdFeatureIds.push_back(fid);
        }
    }

    const auto report = history.recompute();
    if (!report.ok) {
        result.message = report.message;
        return result;
    }

    result.ok = true;
    result.message = "Executed " + std::to_string(result.createdFeatureIds.size()) + " feature(s)";
    return result;
}

} // namespace ms3d::ai
