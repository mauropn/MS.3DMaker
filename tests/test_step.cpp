#include "geometry/SolidFactory.h"
#include "io/StepIO.h"

#include <QFileInfo>
#include <QTemporaryDir>
#include <QtTest/QtTest>

class TestStep : public QObject {
    Q_OBJECT
private slots:
    void exportImportBoxRoundTrip();
    void exportRejectsNullShape();
    void importRejectsMissingFile();
};

void TestStep::exportImportBoxRoundTrip()
{
    const auto box = ms3d::geometry::makeBox(25.0, 40.0, 15.0);
    QVERIFY2(box.ok, box.error.c_str());

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath(QStringLiteral("box_roundtrip.step"));

    const auto trip =
        ms3d::io::roundTripStep(box.solid, path.toStdString(), ms3d::io::StepSchema::AP214);
    QVERIFY2(trip.ok, trip.error.c_str());
    QVERIFY(!trip.shape.IsNull());
    QVERIFY(QFileInfo::exists(path));
}

void TestStep::exportRejectsNullShape()
{
    TopoDS_Shape nullShape;
    const auto result = ms3d::io::exportStep(nullShape, "unused.step");
    QVERIFY(!result.ok);
}

void TestStep::importRejectsMissingFile()
{
    const auto result = ms3d::io::importStep("Z:/definitely/missing/ms3d_step_test.step");
    QVERIFY(!result.ok);
}

QTEST_APPLESS_MAIN(TestStep)
#include "test_step.moc"
