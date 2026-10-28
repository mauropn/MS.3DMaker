#include "AiValidator.h"

#include <unordered_map>
#include <unordered_set>

namespace ms3d::ai {

AiValidationResult validatePlan(const AiPlan& plan) {
    AiValidationResult result;
    if (plan.steps.empty()) {
        result.ok = false;
        result.issues.push_back({"empty_plan", "Plan has no steps"});
        return result;
    }

    std::unordered_set<int> seenIds;
    std::unordered_map<int, AiOpKind> kindById;

    for (const auto& step : plan.steps) {
        if (step.id <= 0) {
            result.ok = false;
            result.issues.push_back({"bad_id", "Step id must be positive"});
            continue;
        }
        if (!seenIds.insert(step.id).second) {
            result.ok = false;
            result.issues.push_back({"duplicate_id", "Duplicate step id"});
        }
        kindById[step.id] = step.op;

        if (isBooleanOp(step.op)) {
            if (step.inputA <= 0 || step.inputB <= 0) {
                result.ok = false;
                result.issues.push_back(
                    {"boolean_inputs", "Boolean step requires inputA and inputB"});
            } else if (seenIds.count(step.inputA) == 0 || seenIds.count(step.inputB) == 0) {
                // inputs must appear earlier in the plan (seen already)
                // If not in seenIds yet, forward reference
                result.ok = false;
                result.issues.push_back(
                    {"boolean_order", "Boolean inputs must refer to earlier plan steps"});
            } else if (step.inputA == step.id || step.inputB == step.id) {
                result.ok = false;
                result.issues.push_back({"boolean_self", "Boolean step cannot reference itself"});
            }
        } else {
            // Primitive: require positive core dimensions where applicable
            auto requirePositive = [&](const char* key) {
                for (const auto& p : step.params) {
                    if (p.key == key) {
                        if (p.value <= 0.0) {
                            result.ok = false;
                            result.issues.push_back(
                                {"non_positive_param",
                                 std::string("Parameter ") + key + " must be positive"});
                        }
                        return;
                    }
                }
                result.ok = false;
                result.issues.push_back(
                    {"missing_param", std::string("Missing required parameter ") + key});
            };
            switch (step.op) {
            case AiOpKind::MakeBox:
                requirePositive("dx");
                requirePositive("dy");
                requirePositive("dz");
                break;
            case AiOpKind::MakeCylinder:
                requirePositive("radius");
                requirePositive("height");
                break;
            case AiOpKind::MakeSphere:
                requirePositive("radius");
                break;
            case AiOpKind::MakeExtrusion:
                requirePositive("width");
                requirePositive("depth");
                requirePositive("height");
                break;
            case AiOpKind::MakeRevolution:
                requirePositive("outerRadius");
                requirePositive("height");
                break;
            default:
                break;
            }
        }
    }

    return result;
}

} // namespace ms3d::ai
