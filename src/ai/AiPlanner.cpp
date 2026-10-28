#include "AiPlanner.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <regex>
#include <sstream>

namespace ms3d::ai {
namespace {

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool extractTripleMm(const std::string& text, double& a, double& b, double& c) {
    // 100 x 60 x 30 or 100×60×30 or 100*60*30
    static const std::regex re(
        R"((\d+(?:\.\d+)?)\s*[x×\*]\s*(\d+(?:\.\d+)?)\s*[x×\*]\s*(\d+(?:\.\d+)?))",
        std::regex::icase);
    std::smatch m;
    if (!std::regex_search(text, m, re)) {
        return false;
    }
    a = std::stod(m[1].str());
    b = std::stod(m[2].str());
    c = std::stod(m[3].str());
    return a > 0 && b > 0 && c > 0;
}

bool extractWallMm(const std::string& text, double& wall) {
    static const std::regex re(R"((\d+(?:\.\d+)?)\s*mm\s*walls?)", std::regex::icase);
    std::smatch m;
    if (!std::regex_search(text, m, re)) {
        // "with 3 mm wall" / "walls of 3"
        static const std::regex re2(R"(walls?\s*(?:of|=|:)?\s*(\d+(?:\.\d+)?)\s*mm)",
                                    std::regex::icase);
        if (!std::regex_search(text, m, re2)) {
            return false;
        }
        wall = std::stod(m[1].str());
        return wall > 0;
    }
    wall = std::stod(m[1].str());
    return wall > 0;
}

bool wantsEnclosure(const std::string& lower) {
    return lower.find("enclosure") != std::string::npos ||
           lower.find("box with") != std::string::npos ||
           (lower.find("walls") != std::string::npos && lower.find("hole") != std::string::npos);
}

bool wantsSimpleBox(const std::string& lower) {
    return lower.find("box") != std::string::npos || lower.find("rectangle") != std::string::npos;
}

bool wantsCylinder(const std::string& lower) {
    return lower.find("cylinder") != std::string::npos || lower.find("tube") != std::string::npos;
}

bool wantsSphere(const std::string& lower) {
    return lower.find("sphere") != std::string::npos || lower.find("ball") != std::string::npos;
}

AiStep makeStep(int id, AiOpKind op, std::string label,
                std::vector<ms3d::features::FeatureParam> params, int a = -1, int b = -1) {
    AiStep s;
    s.id = id;
    s.op = op;
    s.label = std::move(label);
    s.params = std::move(params);
    s.inputA = a;
    s.inputB = b;
    return s;
}

PlanResult planEnclosure(const std::string& prompt, double dx, double dy, double dz, double wall) {
    PlanResult result;
    result.plan.sourcePrompt = prompt;

    if (wall * 2.0 >= dx || wall * 2.0 >= dy || wall >= dz) {
        result.error = "Wall thickness too large for enclosure outer dimensions";
        return result;
    }

    const double innerDx = dx - 2.0 * wall;
    const double innerDy = dy - 2.0 * wall;
    const double innerDz = dz - wall; // open top shell approximation
    const double holeR = std::max(1.5, wall);
    const double margin = wall + holeR + 2.0;

    int next = 1;
    // Outer body
    result.plan.steps.push_back(makeStep(next++, AiOpKind::MakeBox, "Outer enclosure body",
                                         {{"dx", dx}, {"dy", dy}, {"dz", dz}}));
    const int outerId = 1;

    // Inner cavity (inset on XY, from z=wall so bottom remains)
    result.plan.steps.push_back(makeStep(next++, AiOpKind::MakeBox, "Inner cavity volume",
                                         {{"dx", innerDx},
                                          {"dy", innerDy},
                                          {"dz", innerDz},
                                          {"tx", wall},
                                          {"ty", wall},
                                          {"tz", wall}}));
    const int cavityId = 2;

    result.plan.steps.push_back(makeStep(next++, AiOpKind::BooleanCut, "Cut cavity from outer body",
                                         {}, outerId, cavityId));
    int bodyId = 3;

    // Four mounting holes near corners
    const double xs[4] = {margin, dx - margin, margin, dx - margin};
    const double ys[4] = {margin, margin, dy - margin, dy - margin};
    for (int i = 0; i < 4; ++i) {
        const int cylId = next++;
        std::ostringstream label;
        label << "Mounting hole cylinder " << (i + 1);
        result.plan.steps.push_back(makeStep(
            cylId, AiOpKind::MakeCylinder, label.str(),
            {{"radius", holeR}, {"height", dz + 2.0}, {"tx", xs[i]}, {"ty", ys[i]}, {"tz", -1.0}}));
        const int cutId = next++;
        result.plan.steps.push_back(makeStep(cutId, AiOpKind::BooleanCut,
                                             "Cut mounting hole " + std::to_string(i + 1), {},
                                             bodyId, cylId));
        bodyId = cutId;
    }

    result.plan.notes.push_back(
        "Constrained enclosure recipe: open-top shell via cavity cut + four corner holes.");
    result.plan.notes.push_back(
        "Material adjectives (e.g. aluminum) are ignored; geometry only is executed.");
    result.ok = true;
    return result;
}

} // namespace

PlanResult planFromPrompt(const std::string& prompt) {
    PlanResult result;
    result.plan.sourcePrompt = prompt;
    if (prompt.find_first_not_of(" \t\r\n") == std::string::npos) {
        result.error = "Prompt is empty";
        return result;
    }

    const std::string lower = toLower(prompt);
    double dx = 0, dy = 0, dz = 0;

    if (wantsEnclosure(lower) && extractTripleMm(prompt, dx, dy, dz)) {
        double wall = 3.0;
        extractWallMm(lower, wall);
        return planEnclosure(prompt, dx, dy, dz, wall);
    }

    if (wantsSimpleBox(lower) && extractTripleMm(prompt, dx, dy, dz)) {
        result.plan.steps.push_back(
            makeStep(1, AiOpKind::MakeBox, "Box solid", {{"dx", dx}, {"dy", dy}, {"dz", dz}}));
        result.plan.notes.push_back("Parsed as a single axis-aligned box (mm).");
        result.ok = true;
        return result;
    }

    if (wantsCylinder(lower)) {
        static const std::regex reR(R"(r\s*=\s*(\d+(?:\.\d+)?)|radius\s*[:=]?\s*(\d+(?:\.\d+)?))",
                                    std::regex::icase);
        static const std::regex reH(R"(h\s*=\s*(\d+(?:\.\d+)?)|height\s*[:=]?\s*(\d+(?:\.\d+)?))",
                                    std::regex::icase);
        std::smatch m;
        double radius = 25.0;
        double height = 60.0;
        if (std::regex_search(prompt, m, reR)) {
            radius = std::stod(m[1].matched ? m[1].str() : m[2].str());
        }
        if (std::regex_search(prompt, m, reH)) {
            height = std::stod(m[1].matched ? m[1].str() : m[2].str());
        }
        result.plan.steps.push_back(makeStep(1, AiOpKind::MakeCylinder, "Cylinder solid",
                                             {{"radius", radius}, {"height", height}}));
        result.ok = true;
        return result;
    }

    if (wantsSphere(lower)) {
        static const std::regex reR(R"(r\s*=\s*(\d+(?:\.\d+)?)|radius\s*[:=]?\s*(\d+(?:\.\d+)?))",
                                    std::regex::icase);
        std::smatch m;
        double radius = 40.0;
        if (std::regex_search(prompt, m, reR)) {
            radius = std::stod(m[1].matched ? m[1].str() : m[2].str());
        }
        result.plan.steps.push_back(
            makeStep(1, AiOpKind::MakeSphere, "Sphere solid", {{"radius", radius}}));
        result.ok = true;
        return result;
    }

    result.error = "Unsupported prompt. Try: enclosure 100 x 60 x 30 mm with 3 mm walls; "
                   "or box 100 x 60 x 30; or cylinder r=25 h=60; or sphere r=40.";
    return result;
}

} // namespace ms3d::ai
