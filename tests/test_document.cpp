#include "document/Document.h"

#include <QTemporaryDir>
#include <QtTest/QtTest>

class TestDocument : public QObject {
    Q_OBJECT
private slots:
    void newDocumentIsClean();
    void roundTripJsonPreservesFeaturesAndSketch();
    void rejectsCorruptJson();
    void rejectsWrongFormat();
    void rejectsSchemaTooNew();
    void refusesOverwriteNewerSchemaFile();
    void loadMissingFields();
};

void TestDocument::newDocumentIsClean()
{
    ms3d::document::Document doc;
    QVERIFY(!doc.isDirty());
    QCOMPARE(doc.title(), QStringLiteral("Untitled"));
    QVERIFY(doc.history().features().empty());
}

void TestDocument::roundTripJsonPreservesFeaturesAndSketch()
{
    ms3d::document::Document doc;
    doc.history().addPrimitive(ms3d::features::FeatureKind::Box, {{"dx", 10}, {"dy", 20}, {"dz", 30}},
                               "Box");
    doc.sketch().addLine({0, 0}, {10, 0});
    doc.sketch().addConstraint(ms3d::sketch::ConstraintKind::Horizontal,
                               doc.sketch().entities().front().id);
    doc.setTitle(QStringLiteral("DemoPart"));
    QVERIFY(doc.isDirty());

    bool ok = false;
    QString err;
    const QByteArray json = doc.toJson(&ok, &err);
    QVERIFY2(ok, qPrintable(err));
    QVERIFY(!json.isEmpty());

    ms3d::document::Document loaded;
    const auto result = loaded.loadFromJson(json);
    QCOMPARE(result.status, ms3d::document::LoadStatus::Ok);
    QCOMPARE(loaded.title(), QStringLiteral("DemoPart"));
    QCOMPARE(loaded.history().features().size(), size_t(1));
    QCOMPARE(loaded.sketch().entities().size(), size_t(1));
    QCOMPARE(loaded.sketch().constraints().size(), size_t(1));
    QVERIFY(!loaded.isDirty());
    QVERIFY(loaded.history().recompute().ok);
}

void TestDocument::rejectsCorruptJson()
{
    ms3d::document::Document doc;
    const auto result = doc.loadFromJson(QByteArrayLiteral("{not-json"));
    QCOMPARE(result.status, ms3d::document::LoadStatus::CorruptJson);
}

void TestDocument::rejectsWrongFormat()
{
    const QByteArray json = QByteArrayLiteral(
        R"({"format":"OtherCAD","schemaVersion":1,"title":"x","featureState":{"nextId":1,"nameSerial":1},"features":[],"sketch":{"plane":"XY","nextId":1,"entities":[],"constraints":[],"dimensions":[]}})");
    ms3d::document::Document doc;
    const auto result = doc.loadFromJson(json);
    QCOMPARE(result.status, ms3d::document::LoadStatus::WrongFormat);
}

void TestDocument::rejectsSchemaTooNew()
{
    const QByteArray json = QByteArrayLiteral(
        R"({"format":"MS3DMaker","schemaVersion":99,"title":"x","featureState":{"nextId":1,"nameSerial":1},"features":[],"sketch":{"plane":"XY","nextId":1,"entities":[],"constraints":[],"dimensions":[]}})");
    ms3d::document::Document doc;
    const auto result = doc.loadFromJson(json);
    QCOMPARE(result.status, ms3d::document::LoadStatus::SchemaTooNew);
}

void TestDocument::refusesOverwriteNewerSchemaFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString path = dir.filePath(QStringLiteral("newer.ms3d"));
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write(QByteArrayLiteral(
        R"({"format":"MS3DMaker","schemaVersion":99,"title":"x","featureState":{"nextId":1,"nameSerial":1},"features":[],"sketch":{"plane":"XY","nextId":1,"entities":[],"constraints":[],"dimensions":[]}})"));
    f.close();

    ms3d::document::Document doc;
    doc.history().addPrimitive(ms3d::features::FeatureKind::Sphere, {{"radius", 5}}, "Sphere");
    const auto saved = doc.saveToFile(path);
    QVERIFY(!saved.ok);
    QVERIFY(saved.message.contains(QStringLiteral("newer schema"), Qt::CaseInsensitive));
}

void TestDocument::loadMissingFields()
{
    ms3d::document::Document doc;
    const auto result = doc.loadFromJson(QByteArrayLiteral(R"({"format":"MS3DMaker"})"));
    QCOMPARE(result.status, ms3d::document::LoadStatus::MissingFields);
}

QTEST_APPLESS_MAIN(TestDocument)
#include "test_document.moc"
