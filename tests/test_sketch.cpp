#include "sketch/Sketch.h"

#include <QtTest/QtTest>
#include <TopoDS_Compound.hxx>

#include <cmath>

class TestSketch : public QObject {
    Q_OBJECT
private slots:
    void addLineCircleRectangle();
    void horizontalConstraintApplied();
    void verticalConstraintApplied();
    void fixedLengthConstraint();
    void validateDetectsZeroLine();
    void dimensionsCreated();
    void buildDisplayCompoundOk();
    void planeNames();
};

void TestSketch::addLineCircleRectangle() {
    ms3d::sketch::Sketch sketch;
    const int line = sketch.addLine({0, 0}, {10, 0});
    const int circ = sketch.addCircle({5, 5}, 3);
    const int rect = sketch.addRectangle({0, 0}, 20, 10);
    QVERIFY(line > 0);
    QVERIFY(circ > 0);
    QVERIFY(rect > 0);
    QCOMPARE(sketch.entities().size(), size_t(3));
}

void TestSketch::horizontalConstraintApplied() {
    ms3d::sketch::Sketch sketch;
    const int id = sketch.addLine({0, 0}, {10, 4});
    sketch.addConstraint(ms3d::sketch::ConstraintKind::Horizontal, id);
    sketch.applyConstraints();
    const auto e = sketch.findEntity(id);
    QVERIFY(e.has_value());
    QCOMPARE(e->p0.v, e->p1.v);
    QVERIFY(sketch.validate().ok);
}

void TestSketch::verticalConstraintApplied() {
    ms3d::sketch::Sketch sketch;
    const int id = sketch.addLine({0, 0}, {4, 10});
    sketch.addConstraint(ms3d::sketch::ConstraintKind::Vertical, id);
    sketch.applyConstraints();
    const auto e = sketch.findEntity(id);
    QVERIFY(e.has_value());
    QCOMPARE(e->p0.u, e->p1.u);
    QVERIFY(sketch.validate().ok);
}

void TestSketch::fixedLengthConstraint() {
    ms3d::sketch::Sketch sketch;
    const int id = sketch.addLine({0, 0}, {10, 0});
    sketch.addConstraint(ms3d::sketch::ConstraintKind::FixedLength, id, -1, 25.0);
    sketch.applyConstraints();
    const auto e = sketch.findEntity(id);
    QVERIFY(e.has_value());
    QVERIFY(qAbs(std::hypot(e->p1.u - e->p0.u, e->p1.v - e->p0.v) - 25.0) < 1e-6);
    QVERIFY(sketch.validate().ok);
}

void TestSketch::validateDetectsZeroLine() {
    ms3d::sketch::Sketch sketch;
    sketch.addLine({1, 1}, {1, 1});
    QVERIFY(!sketch.validate().ok);
}

void TestSketch::dimensionsCreated() {
    ms3d::sketch::Sketch sketch;
    sketch.addLine({0, 0}, {10, 0});
    sketch.addCircle({0, 0}, 5);
    QVERIFY(sketch.dimensions().size() >= 2);
    for (const auto& d : sketch.dimensions()) {
        QVERIFY(d.value > 0.0);
    }
}

void TestSketch::buildDisplayCompoundOk() {
    ms3d::sketch::Sketch sketch(ms3d::sketch::PlaneId::XY);
    sketch.addLine({0, 0}, {30, 0});
    sketch.addCircle({15, 10}, 5);
    sketch.addRectangle({0, 0}, 20, 10);
    TopoDS_Compound compound;
    std::string err;
    QVERIFY2(sketch.buildDisplayCompound(compound, &err), err.c_str());
    QVERIFY(!compound.IsNull());
}

void TestSketch::planeNames() {
    QCOMPARE(QString(ms3d::sketch::planeIdName(ms3d::sketch::PlaneId::XY)), QString("XY"));
    QCOMPARE(QString(ms3d::sketch::planeIdName(ms3d::sketch::PlaneId::XZ)), QString("XZ"));
    QCOMPARE(QString(ms3d::sketch::planeIdName(ms3d::sketch::PlaneId::YZ)), QString("YZ"));
}

QTEST_APPLESS_MAIN(TestSketch)
#include "test_sketch.moc"
