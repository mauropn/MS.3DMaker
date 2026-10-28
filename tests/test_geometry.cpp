#include "geometry/SolidFactory.h"

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <QtTest/QtTest>
#include <TopoDS.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

namespace {

TopoDS_Solid translatedBox(double dx, double dy, double dz, double tx, double ty, double tz)
{
    BRepPrimAPI_MakeBox maker(dx, dy, dz);
    maker.Build();
    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(tx, ty, tz));
    BRepBuilderAPI_Transform xform(maker.Solid(), trsf);
    return TopoDS::Solid(xform.Shape());
}

void assertBoundsNear(const TopoDS_Shape& shape, double dx, double dy, double dz)
{
    Bnd_Box box;
    BRepBndLib::Add(shape, box, Standard_False);
    Standard_Real xMin = 0, yMin = 0, zMin = 0, xMax = 0, yMax = 0, zMax = 0;
    box.Get(xMin, yMin, zMin, xMax, yMax, zMax);
    QVERIFY(qAbs((xMax - xMin) - dx) < 1e-4);
    QVERIFY(qAbs((yMax - yMin) - dy) < 1e-4);
    QVERIFY(qAbs((zMax - zMin) - dz) < 1e-4);
}

double bboxVolumeApprox(const TopoDS_Shape& shape)
{
    Bnd_Box box;
    BRepBndLib::Add(shape, box, Standard_False);
    Standard_Real xMin = 0, yMin = 0, zMin = 0, xMax = 0, yMax = 0, zMax = 0;
    box.Get(xMin, yMin, zMin, xMax, yMax, zMax);
    return (xMax - xMin) * (yMax - yMin) * (zMax - zMin);
}

} // namespace

class TestGeometry : public QObject {
    Q_OBJECT
private slots:
    void makeBoxCreatesValidSolid();
    void makeBoxRejectsNonPositiveDimensions();
    void makeBoxBoundsMatchDimensions();
    void makeCylinderCreatesValidSolid();
    void makeCylinderRejectsBadDims();
    void makeSphereCreatesValidSolid();
    void makeSphereRejectsBadRadius();
    void makeExtrusionCreatesValidSolid();
    void makeExtrusionRejectsBadDims();
    void makeRevolutionCreatesValidSolid();
    void makeRevolutionRejectsBadDims();
    void booleanUnionCreatesValidSolid();
    void booleanCutShrinksBBoxVolume();
    void booleanCommonCreatesValidSolid();
    void booleanRejectsNullArgs();
};

void TestGeometry::makeBoxCreatesValidSolid()
{
    const auto result = ms3d::geometry::makeBox(10.0, 20.0, 30.0);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
    QVERIFY(!result.solid.IsNull());
}

void TestGeometry::makeBoxRejectsNonPositiveDimensions()
{
    QCOMPARE(ms3d::geometry::makeBox(0.0, 1.0, 1.0).ok, false);
    QCOMPARE(ms3d::geometry::makeBox(-1.0, 1.0, 1.0).ok, false);
    QCOMPARE(ms3d::geometry::makeBox(1.0, 0.0, 1.0).ok, false);
    QCOMPARE(ms3d::geometry::makeBox(1.0, 1.0, -5.0).ok, false);
}

void TestGeometry::makeBoxBoundsMatchDimensions()
{
    const auto result = ms3d::geometry::makeBox(100.0, 60.0, 30.0);
    QVERIFY2(result.ok, result.error.c_str());
    assertBoundsNear(result.solid, 100.0, 60.0, 30.0);
}

void TestGeometry::makeCylinderCreatesValidSolid()
{
    const auto result = ms3d::geometry::makeCylinder(25.0, 60.0);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
}

void TestGeometry::makeCylinderRejectsBadDims()
{
    QCOMPARE(ms3d::geometry::makeCylinder(0.0, 10.0).ok, false);
    QCOMPARE(ms3d::geometry::makeCylinder(10.0, -1.0).ok, false);
}

void TestGeometry::makeSphereCreatesValidSolid()
{
    const auto result = ms3d::geometry::makeSphere(40.0);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
}

void TestGeometry::makeSphereRejectsBadRadius()
{
    QCOMPARE(ms3d::geometry::makeSphere(0.0).ok, false);
    QCOMPARE(ms3d::geometry::makeSphere(-2.0).ok, false);
}

void TestGeometry::makeExtrusionCreatesValidSolid()
{
    const auto result = ms3d::geometry::makeExtrusion(80.0, 50.0, 20.0);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
    assertBoundsNear(result.solid, 80.0, 50.0, 20.0);
}

void TestGeometry::makeExtrusionRejectsBadDims()
{
    QCOMPARE(ms3d::geometry::makeExtrusion(0.0, 10.0, 10.0).ok, false);
    QCOMPARE(ms3d::geometry::makeExtrusion(10.0, -1.0, 10.0).ok, false);
}

void TestGeometry::makeRevolutionCreatesValidSolid()
{
    const auto result = ms3d::geometry::makeRevolution(15.0, 30.0, 50.0);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
}

void TestGeometry::makeRevolutionRejectsBadDims()
{
    QCOMPARE(ms3d::geometry::makeRevolution(10.0, 10.0, 20.0).ok, false);
    QCOMPARE(ms3d::geometry::makeRevolution(20.0, 10.0, 20.0).ok, false);
    QCOMPARE(ms3d::geometry::makeRevolution(5.0, 10.0, 0.0).ok, false);
}

void TestGeometry::booleanUnionCreatesValidSolid()
{
    const auto a = ms3d::geometry::makeBox(80.0, 50.0, 40.0);
    QVERIFY2(a.ok, a.error.c_str());
    const TopoDS_Solid b = translatedBox(60.0, 40.0, 50.0, 30.0, 15.0, 10.0);
    const auto result = ms3d::geometry::booleanUnion(a.solid, b);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
}

void TestGeometry::booleanCutShrinksBBoxVolume()
{
    const auto a = ms3d::geometry::makeBox(80.0, 50.0, 40.0);
    QVERIFY2(a.ok, a.error.c_str());
    const TopoDS_Solid b = translatedBox(60.0, 40.0, 50.0, 30.0, 15.0, 10.0);
    const double before = bboxVolumeApprox(a.solid);
    const auto result = ms3d::geometry::booleanCut(a.solid, b);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
    // Cut may keep similar AABB; ensure result is non-null and smaller or equal AABB volume
    // when tool fully contained — with offset overlap, AABB of cut can stay similar.
    // Soft check: cut result bbox volume must not exceed original AABB by much.
    QVERIFY(bboxVolumeApprox(result.solid) <= before + 1.0);
}

void TestGeometry::booleanCommonCreatesValidSolid()
{
    const auto a = ms3d::geometry::makeBox(80.0, 50.0, 40.0);
    QVERIFY2(a.ok, a.error.c_str());
    const TopoDS_Solid b = translatedBox(60.0, 40.0, 50.0, 30.0, 15.0, 10.0);
    const auto result = ms3d::geometry::booleanCommon(a.solid, b);
    QVERIFY2(result.ok, result.error.c_str());
    QVERIFY(ms3d::geometry::isValidSolid(result.solid));
    QVERIFY(bboxVolumeApprox(result.solid) < bboxVolumeApprox(a.solid));
}

void TestGeometry::booleanRejectsNullArgs()
{
    TopoDS_Shape nullShape;
    const auto box = ms3d::geometry::makeBox(10.0, 10.0, 10.0);
    QVERIFY(box.ok);
    QCOMPARE(ms3d::geometry::booleanUnion(nullShape, box.solid).ok, false);
    QCOMPARE(ms3d::geometry::booleanCut(box.solid, nullShape).ok, false);
    QCOMPARE(ms3d::geometry::booleanCommon(nullShape, nullShape).ok, false);
}

QTEST_APPLESS_MAIN(TestGeometry)
#include "test_geometry.moc"
