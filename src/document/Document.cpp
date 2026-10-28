#include "Document.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

namespace ms3d::document {
namespace {

QJsonObject vec2ToJson(const ms3d::sketch::Vec2& v) {
    QJsonObject o;
    o.insert(QStringLiteral("u"), v.u);
    o.insert(QStringLiteral("v"), v.v);
    return o;
}

bool vec2FromJson(const QJsonObject& o, ms3d::sketch::Vec2& v) {
    if (!o.contains(QStringLiteral("u")) || !o.contains(QStringLiteral("v"))) {
        return false;
    }
    v.u = o.value(QStringLiteral("u")).toDouble();
    v.v = o.value(QStringLiteral("v")).toDouble();
    return true;
}

} // namespace

Document::Document() {
    newDocument();
}

void Document::setTitle(const QString& title) {
    if (m_title == title) {
        return;
    }
    m_title = title;
    markDirty();
}

void Document::markDirty() {
    m_dirty = true;
}

void Document::clearDirty() {
    m_dirty = false;
}

void Document::newDocument() {
    m_history.clear();
    m_sketch.clear();
    m_sketch.setPlane(ms3d::sketch::PlaneId::XY);
    m_filePath.clear();
    m_title = QStringLiteral("Untitled");
    m_dirty = false;
}

QString Document::statusMessage(LoadStatus status) {
    switch (status) {
    case LoadStatus::Ok:
        return QStringLiteral("OK");
    case LoadStatus::IoError:
        return QStringLiteral("I/O error reading or writing the document");
    case LoadStatus::CorruptJson:
        return QStringLiteral("Document JSON is corrupt or unreadable");
    case LoadStatus::MissingFields:
        return QStringLiteral("Document is missing required fields");
    case LoadStatus::WrongFormat:
        return QStringLiteral("File is not an MS3DMaker document");
    case LoadStatus::SchemaTooNew:
        return QStringLiteral("Document schema is newer than this application supports");
    case LoadStatus::SchemaUnsupported:
        return QStringLiteral("Document schema version is not supported");
    case LoadStatus::InvalidContent:
        return QStringLiteral("Document content failed validation");
    }
    return QStringLiteral("Unknown document error");
}

QByteArray Document::toJson(bool* ok, QString* errorOut) const {
    QJsonObject root;
    root.insert(QStringLiteral("format"), QString::fromUtf8(kFormatId));
    root.insert(QStringLiteral("schemaVersion"), kSchemaVersion);
    root.insert(QStringLiteral("title"), m_title);

    QJsonObject featureState;
    featureState.insert(QStringLiteral("nextId"), m_history.peekNextId());
    featureState.insert(QStringLiteral("nameSerial"), m_history.peekNameSerial());
    root.insert(QStringLiteral("featureState"), featureState);

    QJsonArray features;
    for (const auto& f : m_history.features()) {
        QJsonObject fo;
        fo.insert(QStringLiteral("id"), f.id);
        fo.insert(QStringLiteral("kind"),
                  QString::fromUtf8(ms3d::features::featureKindName(f.kind)));
        fo.insert(QStringLiteral("name"), QString::fromStdString(f.name));
        QJsonArray params;
        for (const auto& p : f.params) {
            QJsonObject po;
            po.insert(QStringLiteral("key"), QString::fromStdString(p.key));
            po.insert(QStringLiteral("value"), p.value);
            params.append(po);
        }
        fo.insert(QStringLiteral("params"), params);
        QJsonArray deps;
        for (int d : f.dependencyIds) {
            deps.append(d);
        }
        fo.insert(QStringLiteral("dependencies"), deps);
        features.append(fo);
    }
    root.insert(QStringLiteral("features"), features);

    QJsonObject sketch;
    sketch.insert(QStringLiteral("plane"),
                  QString::fromUtf8(ms3d::sketch::planeIdName(m_sketch.plane())));
    sketch.insert(QStringLiteral("nextId"), m_sketch.peekNextId());

    QJsonArray entities;
    for (const auto& e : m_sketch.entities()) {
        QJsonObject eo;
        eo.insert(QStringLiteral("id"), e.id);
        eo.insert(QStringLiteral("kind"), QString::fromUtf8(ms3d::sketch::entityKindName(e.kind)));
        eo.insert(QStringLiteral("p0"), vec2ToJson(e.p0));
        eo.insert(QStringLiteral("p1"), vec2ToJson(e.p1));
        eo.insert(QStringLiteral("w"), e.w);
        eo.insert(QStringLiteral("h"), e.h);
        eo.insert(QStringLiteral("r"), e.r);
        entities.append(eo);
    }
    sketch.insert(QStringLiteral("entities"), entities);

    QJsonArray constraints;
    for (const auto& c : m_sketch.constraints()) {
        QJsonObject co;
        co.insert(QStringLiteral("id"), c.id);
        co.insert(QStringLiteral("kind"),
                  QString::fromUtf8(ms3d::sketch::constraintKindName(c.kind)));
        co.insert(QStringLiteral("entityA"), c.entityA);
        co.insert(QStringLiteral("entityB"), c.entityB);
        co.insert(QStringLiteral("value"), c.value);
        constraints.append(co);
    }
    sketch.insert(QStringLiteral("constraints"), constraints);

    QJsonArray dimensions;
    for (const auto& d : m_sketch.dimensions()) {
        QJsonObject dout;
        dout.insert(QStringLiteral("id"), d.id);
        dout.insert(QStringLiteral("kind"),
                    QString::fromUtf8(ms3d::sketch::dimensionKindName(d.kind)));
        dout.insert(QStringLiteral("entityId"), d.entityId);
        dout.insert(QStringLiteral("value"), d.value);
        dimensions.append(dout);
    }
    sketch.insert(QStringLiteral("dimensions"), dimensions);
    root.insert(QStringLiteral("sketch"), sketch);

    const QJsonDocument doc(root);
    if (ok) {
        *ok = true;
    }
    if (errorOut) {
        errorOut->clear();
    }
    return doc.toJson(QJsonDocument::Indented);
}

LoadResult Document::loadFromJson(const QByteArray& bytes) {
    LoadResult result;
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(bytes, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        result.status = LoadStatus::CorruptJson;
        result.message =
            statusMessage(result.status) + QStringLiteral(": ") + parseError.errorString();
        return result;
    }

    const QJsonObject root = doc.object();
    if (!root.contains(QStringLiteral("format")) ||
        !root.contains(QStringLiteral("schemaVersion")) ||
        !root.contains(QStringLiteral("features")) || !root.contains(QStringLiteral("sketch")) ||
        !root.contains(QStringLiteral("featureState"))) {
        result.status = LoadStatus::MissingFields;
        result.message = statusMessage(result.status);
        return result;
    }

    if (root.value(QStringLiteral("format")).toString() != QString::fromUtf8(kFormatId)) {
        result.status = LoadStatus::WrongFormat;
        result.message = statusMessage(result.status);
        return result;
    }

    const int schema = root.value(QStringLiteral("schemaVersion")).toInt(-1);
    result.fileSchemaVersion = schema;
    if (schema < 1) {
        result.status = LoadStatus::SchemaUnsupported;
        result.message = statusMessage(result.status);
        return result;
    }
    if (schema > kSchemaVersion) {
        result.status = LoadStatus::SchemaTooNew;
        result.message = statusMessage(result.status) +
                         QStringLiteral(" (file=%1 app=%2)").arg(schema).arg(kSchemaVersion);
        return result;
    }
    // Backward compatibility: schema == 1 is current; older supported schemas would migrate here.
    if (schema != kSchemaVersion) {
        // Reserved for future migrations; refuse unknown past versions for now.
        result.status = LoadStatus::SchemaUnsupported;
        result.message = statusMessage(result.status) + QStringLiteral(" (file=%1)").arg(schema);
        return result;
    }

    const QJsonObject featureState = root.value(QStringLiteral("featureState")).toObject();
    const int nextId = featureState.value(QStringLiteral("nextId")).toInt(1);
    const int nameSerial = featureState.value(QStringLiteral("nameSerial")).toInt(1);

    std::vector<ms3d::features::Feature> features;
    const QJsonArray featureArr = root.value(QStringLiteral("features")).toArray();
    for (const QJsonValue& item : featureArr) {
        if (!item.isObject()) {
            result.status = LoadStatus::InvalidContent;
            result.message = QStringLiteral("Feature entry is not an object");
            return result;
        }
        const QJsonObject fo = item.toObject();
        ms3d::features::Feature f;
        f.id = fo.value(QStringLiteral("id")).toInt(-1);
        f.name = fo.value(QStringLiteral("name")).toString().toStdString();
        const QString kindName = fo.value(QStringLiteral("kind")).toString();
        if (f.id <= 0 || !ms3d::features::featureKindFromName(kindName.toStdString(), f.kind)) {
            result.status = LoadStatus::InvalidContent;
            result.message = QStringLiteral("Invalid feature kind or id");
            return result;
        }
        const QJsonArray params = fo.value(QStringLiteral("params")).toArray();
        for (const QJsonValue& pv : params) {
            const QJsonObject po = pv.toObject();
            ms3d::features::FeatureParam p;
            p.key = po.value(QStringLiteral("key")).toString().toStdString();
            p.value = po.value(QStringLiteral("value")).toDouble();
            if (p.key.empty()) {
                result.status = LoadStatus::InvalidContent;
                result.message = QStringLiteral("Feature parameter missing key");
                return result;
            }
            f.params.push_back(p);
        }
        const QJsonArray deps = fo.value(QStringLiteral("dependencies")).toArray();
        for (const QJsonValue& dv : deps) {
            f.dependencyIds.push_back(dv.toInt());
        }
        f.dirty = true;
        features.push_back(f);
    }

    const QJsonObject sketchObj = root.value(QStringLiteral("sketch")).toObject();
    ms3d::sketch::PlaneId plane = ms3d::sketch::PlaneId::XY;
    if (!ms3d::sketch::planeIdFromName(
            sketchObj.value(QStringLiteral("plane")).toString().toStdString(), plane)) {
        result.status = LoadStatus::InvalidContent;
        result.message = QStringLiteral("Invalid sketch plane");
        return result;
    }
    const int sketchNextId = sketchObj.value(QStringLiteral("nextId")).toInt(1);

    std::vector<ms3d::sketch::SketchEntity> entities;
    for (const QJsonValue& item : sketchObj.value(QStringLiteral("entities")).toArray()) {
        const QJsonObject eo = item.toObject();
        ms3d::sketch::SketchEntity e;
        e.id = eo.value(QStringLiteral("id")).toInt(-1);
        if (e.id <= 0 || !ms3d::sketch::entityKindFromName(
                             eo.value(QStringLiteral("kind")).toString().toStdString(), e.kind)) {
            result.status = LoadStatus::InvalidContent;
            result.message = QStringLiteral("Invalid sketch entity");
            return result;
        }
        if (!vec2FromJson(eo.value(QStringLiteral("p0")).toObject(), e.p0) ||
            !vec2FromJson(eo.value(QStringLiteral("p1")).toObject(), e.p1)) {
            result.status = LoadStatus::InvalidContent;
            result.message = QStringLiteral("Sketch entity missing coordinates");
            return result;
        }
        e.w = eo.value(QStringLiteral("w")).toDouble();
        e.h = eo.value(QStringLiteral("h")).toDouble();
        e.r = eo.value(QStringLiteral("r")).toDouble();
        entities.push_back(e);
    }

    std::vector<ms3d::sketch::SketchConstraint> constraints;
    for (const QJsonValue& item : sketchObj.value(QStringLiteral("constraints")).toArray()) {
        const QJsonObject co = item.toObject();
        ms3d::sketch::SketchConstraint c;
        c.id = co.value(QStringLiteral("id")).toInt(-1);
        if (c.id <= 0 || !ms3d::sketch::constraintKindFromName(
                             co.value(QStringLiteral("kind")).toString().toStdString(), c.kind)) {
            result.status = LoadStatus::InvalidContent;
            result.message = QStringLiteral("Invalid sketch constraint");
            return result;
        }
        c.entityA = co.value(QStringLiteral("entityA")).toInt(-1);
        c.entityB = co.value(QStringLiteral("entityB")).toInt(-1);
        c.value = co.value(QStringLiteral("value")).toDouble();
        constraints.push_back(c);
    }

    std::vector<ms3d::sketch::SketchDimension> dimensions;
    for (const QJsonValue& item : sketchObj.value(QStringLiteral("dimensions")).toArray()) {
        const QJsonObject dout = item.toObject();
        ms3d::sketch::SketchDimension d;
        d.id = dout.value(QStringLiteral("id")).toInt(-1);
        if (d.id <= 0 || !ms3d::sketch::dimensionKindFromName(
                             dout.value(QStringLiteral("kind")).toString().toStdString(), d.kind)) {
            result.status = LoadStatus::InvalidContent;
            result.message = QStringLiteral("Invalid sketch dimension");
            return result;
        }
        d.entityId = dout.value(QStringLiteral("entityId")).toInt(-1);
        d.value = dout.value(QStringLiteral("value")).toDouble();
        dimensions.push_back(d);
    }

    // Commit only after full validation — never partially overwrite in-memory doc on failure.
    m_history.replaceContents(std::move(features), nextId, nameSerial);
    m_sketch.replaceContents(plane, std::move(entities), std::move(constraints),
                             std::move(dimensions), sketchNextId);
    m_title = root.value(QStringLiteral("title")).toString(QStringLiteral("Untitled"));
    m_dirty = false;
    result.status = LoadStatus::Ok;
    result.message = statusMessage(LoadStatus::Ok);
    return result;
}

LoadResult Document::loadFromFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        LoadResult result;
        result.status = LoadStatus::IoError;
        result.message = statusMessage(result.status) + QStringLiteral(": ") + file.errorString();
        return result;
    }
    const QByteArray bytes = file.readAll();
    LoadResult result = loadFromJson(bytes);
    if (result.status == LoadStatus::Ok) {
        m_filePath = QFileInfo(path).absoluteFilePath();
        if (m_title.isEmpty() || m_title == QStringLiteral("Untitled")) {
            m_title = QFileInfo(path).completeBaseName();
        }
        m_dirty = false;
    }
    return result;
}

SaveResult Document::saveToFile(const QString& path) {
    SaveResult result;
    // Refuse writing over a different/incompatible existing MS3D file without loading it first
    // via Open — if target exists, probe schema and refuse newer/foreign formats.
    if (QFileInfo::exists(path)) {
        QFile probe(path);
        if (probe.open(QIODevice::ReadOnly)) {
            const QByteArray existing = probe.readAll();
            probe.close();
            QJsonParseError parseError;
            const QJsonDocument existingDoc = QJsonDocument::fromJson(existing, &parseError);
            if (parseError.error == QJsonParseError::NoError && existingDoc.isObject()) {
                const QJsonObject root = existingDoc.object();
                const QString format = root.value(QStringLiteral("format")).toString();
                const int schema = root.value(QStringLiteral("schemaVersion")).toInt(-1);
                if (!format.isEmpty() && format != QString::fromUtf8(kFormatId)) {
                    result.message =
                        QStringLiteral(
                            "Refusing to overwrite a non-MS3DMaker file that declares format=") +
                        format;
                    return result;
                }
                if (schema > kSchemaVersion) {
                    result.message =
                        QStringLiteral(
                            "Refusing to overwrite a newer schema document (file=%1 app=%2)")
                            .arg(schema)
                            .arg(kSchemaVersion);
                    return result;
                }
            }
            // Corrupt/unknown existing files: still allow Save As overwrite after user chose path
            // only when format marker is absent — treated as non-MS3DMaker binary/text.
        }
    }

    bool ok = false;
    QString err;
    const QByteArray json = toJson(&ok, &err);
    if (!ok) {
        result.message = err.isEmpty() ? QStringLiteral("Failed to serialize document") : err;
        return result;
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        result.message =
            statusMessage(LoadStatus::IoError) + QStringLiteral(": ") + file.errorString();
        return result;
    }
    if (file.write(json) != json.size()) {
        result.message = QStringLiteral("Incomplete write to document file");
        return result;
    }
    m_filePath = QFileInfo(path).absoluteFilePath();
    m_title = QFileInfo(path).completeBaseName();
    m_dirty = false;
    result.ok = true;
    result.message = QStringLiteral("Saved");
    return result;
}

} // namespace ms3d::document
