#include "assembly/Assembly.h"
#include "geometry/SolidFactory.h"

#include <QtTest/QtTest>
#include <TopExp_Explorer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

class TestAssembly : public QObject {
    Q_OBJECT
private slots:
    void addTwoTranslatedBoxesBuildsCompound();
    void emptyAssemblyFailsBuild();
    void rejectNullShape();
};

void TestAssembly::addTwoTranslatedBoxesBuildsCompound()
{
    const auto box = ms3d::geometry::makeBox(20, 20, 20);
    QVERIFY2(box.ok, box.error.c_str());

    ms3d::assembly::Assembly assembly;
    gp_Trsf a;
    gp_Trsf b;
    b.SetTranslation(gp_Vec(50, 0, 0));
    QVERIFY(assembly.addInstance(box.solid, a) > 0);
    QVERIFY(assembly.addInstance(box.solid, b) > 0);
    QCOMPARE(assembly.instances().size(), size_t(2));

    const auto built = assembly.buildCompound();
    QVERIFY2(built.ok, built.error.c_str());
    QCOMPARE(built.instanceCount, 2);

    int solids = 0;
    for (TopExp_Explorer exp(built.compound, TopAbs_SOLID); exp.More(); exp.Next()) {
        ++solids;
    }
    QCOMPARE(solids, 2);
}

void TestAssembly::emptyAssemblyFailsBuild()
{
    ms3d::assembly::Assembly assembly;
    const auto built = assembly.buildCompound();
    QVERIFY(!built.ok);
}

void TestAssembly::rejectNullShape()
{
    ms3d::assembly::Assembly assembly;
    TopoDS_Shape nullShape;
    gp_Trsf identity;
    QCOMPARE(assembly.addInstance(nullShape, identity), -1);
}

QTEST_APPLESS_MAIN(TestAssembly)
#include "test_assembly.moc"
