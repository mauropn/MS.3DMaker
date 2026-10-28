#include "geometry/SolidFactory.h"

#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <QtTest/QtTest>

class TestGeometry : public QObject {
    Q_OBJECT
private slots:
    void makeBoxCreatesValidSolid();
    void makeBoxRejectsNonPositiveDimensions();
    void makeBoxBoundsMatchDimensions();
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
    const double dx = 100.0;
    const double dy = 60.0;
    const double dz = 30.0;
    const auto result = ms3d::geometry::makeBox(dx, dy, dz);
    QVERIFY2(result.ok, result.error.c_str());

    Bnd_Box box;
    BRepBndLib::Add(result.solid, box, Standard_False);
    Standard_Real xMin = 0, yMin = 0, zMin = 0, xMax = 0, yMax = 0, zMax = 0;
    box.Get(xMin, yMin, zMin, xMax, yMax, zMax);
    QVERIFY(qAbs((xMax - xMin) - dx) < 1e-4);
    QVERIFY(qAbs((yMax - yMin) - dy) < 1e-4);
    QVERIFY(qAbs((zMax - zMin) - dz) < 1e-4);
}

QTEST_APPLESS_MAIN(TestGeometry)
#include "test_geometry.moc"
