#include "drawing/DrawingViews.h"
#include "geometry/SolidFactory.h"

#include <QtTest/QtTest>
#include <TopoDS_Iterator.hxx>

class TestDrawing : public QObject {
    Q_OBJECT
private slots:
    void frontHlrProducesEdges();
    void allOrientationsSucceed();
    void nullShapeFails();
};

void TestDrawing::frontHlrProducesEdges()
{
    const auto box = ms3d::geometry::makeBox(30, 20, 10);
    QVERIFY2(box.ok, box.error.c_str());
    const auto view =
        ms3d::drawing::makeHlrView(box.solid, ms3d::drawing::ViewOrientation::Front);
    QVERIFY2(view.ok, view.error.c_str());
    TopoDS_Iterator it(view.edges);
    QVERIFY(it.More());
}

void TestDrawing::allOrientationsSucceed()
{
    const auto box = ms3d::geometry::makeBox(15, 15, 15);
    QVERIFY(box.ok);
    const ms3d::drawing::ViewOrientation orientations[] = {
        ms3d::drawing::ViewOrientation::Front, ms3d::drawing::ViewOrientation::Top,
        ms3d::drawing::ViewOrientation::Right, ms3d::drawing::ViewOrientation::Iso};
    for (auto orientation : orientations) {
        const auto view = ms3d::drawing::makeHlrView(box.solid, orientation);
        QVERIFY2(view.ok, view.error.c_str());
    }
}

void TestDrawing::nullShapeFails()
{
    TopoDS_Shape nullShape;
    const auto view =
        ms3d::drawing::makeHlrView(nullShape, ms3d::drawing::ViewOrientation::Front);
    QVERIFY(!view.ok);
}

QTEST_APPLESS_MAIN(TestDrawing)
#include "test_drawing.moc"
