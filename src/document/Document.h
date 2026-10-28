#pragma once

#include "features/FeatureHistory.h"
#include "sketch/Sketch.h"

#include <QString>

namespace ms3d::document {

inline constexpr int kSchemaVersion = 1;
inline constexpr const char* kFormatId = "MS3DMaker";
inline constexpr const char* kFileExtension = "ms3d";

enum class LoadStatus {
    Ok,
    IoError,
    CorruptJson,
    MissingFields,
    WrongFormat,
    SchemaTooNew,
    SchemaUnsupported,
    InvalidContent
};

struct LoadResult {
    LoadStatus status = LoadStatus::Ok;
    QString message; // English technical for logs; UI may wrap
    int fileSchemaVersion = 0;
};

struct SaveResult {
    bool ok = false;
    QString message;
};

/// In-memory document: feature history + sketch + path/dirty metadata.
class Document {
public:
    Document();

    ms3d::features::FeatureHistory& history() { return m_history; }
    const ms3d::features::FeatureHistory& history() const { return m_history; }
    ms3d::sketch::Sketch& sketch() { return m_sketch; }
    const ms3d::sketch::Sketch& sketch() const { return m_sketch; }

    QString filePath() const { return m_filePath; }
    QString title() const { return m_title; }
    void setTitle(const QString& title);

    bool isDirty() const { return m_dirty; }
    void markDirty();
    void clearDirty();

    void newDocument();

    LoadResult loadFromFile(const QString& path);
    LoadResult loadFromJson(const QByteArray& bytes);
    SaveResult saveToFile(const QString& path);
    QByteArray toJson(bool* ok = nullptr, QString* errorOut = nullptr) const;

    static QString statusMessage(LoadStatus status);

private:
    ms3d::features::FeatureHistory m_history;
    ms3d::sketch::Sketch m_sketch;
    QString m_filePath;
    QString m_title;
    bool m_dirty = false;
};

} // namespace ms3d::document
