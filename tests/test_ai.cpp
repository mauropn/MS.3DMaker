#include "ai/AiExecutor.h"
#include "ai/AiPlanner.h"
#include "ai/AiValidator.h"
#include "features/FeatureHistory.h"

#include <QtTest/QtTest>

class TestAi : public QObject {
    Q_OBJECT
private slots:
    void planEnclosureExample();
    void planSimpleBox();
    void rejectEmptyPrompt();
    void validateAndExecuteEnclosure();
    void validatorCatchesBadBooleanOrder();
};

void TestAi::planEnclosureExample()
{
    const auto planned = ms3d::ai::planFromPrompt(
        "Create an aluminum enclosure measuring 100 x 60 x 30 mm, with 3 mm walls and four "
        "mounting holes.");
    QVERIFY2(planned.ok, planned.error.c_str());
    QVERIFY(planned.plan.steps.size() >= 5);
    const auto validation = ms3d::ai::validatePlan(planned.plan);
    QVERIFY(validation.ok);
}

void TestAi::planSimpleBox()
{
    const auto planned = ms3d::ai::planFromPrompt("box 80 x 40 x 20");
    QVERIFY(planned.ok);
    QCOMPARE(planned.plan.steps.size(), size_t(1));
    QCOMPARE(planned.plan.steps.front().op, ms3d::ai::AiOpKind::MakeBox);
}

void TestAi::rejectEmptyPrompt()
{
    const auto planned = ms3d::ai::planFromPrompt("   ");
    QVERIFY(!planned.ok);
}

void TestAi::validateAndExecuteEnclosure()
{
    const auto planned = ms3d::ai::planFromPrompt(
        "enclosure 100 x 60 x 30 mm with 3 mm walls and four mounting holes");
    QVERIFY(planned.ok);
    QVERIFY(ms3d::ai::validatePlan(planned.plan).ok);

    ms3d::features::FeatureHistory history;
    const auto executed = ms3d::ai::executePlan(history, planned.plan);
    QVERIFY2(executed.ok, executed.message.c_str());
    QVERIFY(!executed.createdFeatureIds.empty());
    const auto tip = history.tip();
    QVERIFY(tip);
    QVERIFY(tip->valid);
}

void TestAi::validatorCatchesBadBooleanOrder()
{
    ms3d::ai::AiPlan plan;
    ms3d::ai::AiStep cut;
    cut.id = 1;
    cut.op = ms3d::ai::AiOpKind::BooleanCut;
    cut.inputA = 2;
    cut.inputB = 3;
    plan.steps.push_back(cut);
    const auto validation = ms3d::ai::validatePlan(plan);
    QVERIFY(!validation.ok);
}

QTEST_APPLESS_MAIN(TestAi)
#include "test_ai.moc"
