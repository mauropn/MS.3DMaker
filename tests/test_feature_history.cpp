#include "features/FeatureHistory.h"

#include <QtTest/QtTest>

class TestFeatureHistory : public QObject {
    Q_OBJECT
private slots:
    void addAndRecomputeBox();
    void editParameterInvalidatesAndRecomputes();
    void booleanDependsOnBoxes();
    void failedUpstreamMarksDependent();
    void dependentsCollected();
    void noCycleOnSimpleGraph();
};

void TestFeatureHistory::addAndRecomputeBox()
{
    ms3d::features::FeatureHistory history;
    const int id = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                        {{"dx", 10}, {"dy", 20}, {"dz", 30}}, "Box");
    QVERIFY(id > 0);
    const auto report = history.recompute();
    QVERIFY2(report.ok, report.message.c_str());
    const auto f = history.find(id);
    QVERIFY(f);
    QVERIFY(f->valid);
    QVERIFY(!f->dirty);
}

void TestFeatureHistory::editParameterInvalidatesAndRecomputes()
{
    ms3d::features::FeatureHistory history;
    const int id = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                        {{"dx", 10}, {"dy", 10}, {"dz", 10}}, "Box");
    QVERIFY(history.recompute().ok);
    QVERIFY(history.setParameter(id, "dx", 50));
    const auto dirty = history.find(id);
    QVERIFY(dirty);
    QVERIFY(dirty->dirty);
    QVERIFY(history.recompute().ok);
    const auto updated = history.find(id);
    QVERIFY(updated);
    QVERIFY(updated->valid);
    double dx = 0;
    for (const auto& p : updated->params) {
        if (p.key == "dx") {
            dx = p.value;
        }
    }
    QCOMPARE(dx, 50.0);
}

void TestFeatureHistory::booleanDependsOnBoxes()
{
    ms3d::features::FeatureHistory history;
    const int a = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                       {{"dx", 80}, {"dy", 50}, {"dz", 40}}, "Box");
    const int b = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                       {{"dx", 60}, {"dy", 40}, {"dz", 50}}, "Box");
    const int u = history.addBoolean(ms3d::features::FeatureKind::BooleanUnion, a, b, "Union");
    QVERIFY(u > 0);
    const auto report = history.recompute();
    QVERIFY2(report.ok, report.message.c_str());
    const auto tip = history.tip();
    QVERIFY(tip);
    QCOMPARE(tip->id, u);
    QVERIFY(tip->valid);
}

void TestFeatureHistory::failedUpstreamMarksDependent()
{
    ms3d::features::FeatureHistory history;
    const int a = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                       {{"dx", -1}, {"dy", 10}, {"dz", 10}}, "Box");
    const int b = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                       {{"dx", 10}, {"dy", 10}, {"dz", 10}}, "Box");
    const int cut = history.addBoolean(ms3d::features::FeatureKind::BooleanCut, a, b, "Cut");
    const auto report = history.recompute();
    QVERIFY(!report.ok);
    const auto fa = history.find(a);
    const auto fcut = history.find(cut);
    QVERIFY(fa);
    QVERIFY(!fa->valid);
    QVERIFY(fcut);
    QVERIFY(!fcut->valid);
}

void TestFeatureHistory::dependentsCollected()
{
    ms3d::features::FeatureHistory history;
    const int a = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                       {{"dx", 10}, {"dy", 10}, {"dz", 10}}, "Box");
    const int b = history.addPrimitive(ms3d::features::FeatureKind::Box,
                                       {{"dx", 5}, {"dy", 5}, {"dz", 5}}, "Box");
    const int u = history.addBoolean(ms3d::features::FeatureKind::BooleanUnion, a, b, "Union");
    const auto deps = history.directDependents(a);
    QCOMPARE(deps.size(), size_t(1));
    QCOMPARE(deps[0], u);
}

void TestFeatureHistory::noCycleOnSimpleGraph()
{
    ms3d::features::FeatureHistory history;
    const int a = history.addPrimitive(ms3d::features::FeatureKind::Cylinder,
                                       {{"radius", 5}, {"height", 10}}, "Cyl");
    QVERIFY(a > 0);
    QVERIFY(!history.hasCycle());
    QVERIFY(history.recompute().ok);
}

QTEST_APPLESS_MAIN(TestFeatureHistory)
#include "test_feature_history.moc"
