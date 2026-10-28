#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <memory>

class QCoreApplication;

namespace ms3d::i18n {

struct LocaleInfo {
    QString id;           // en, pt-BR
    QString nativeName;   // English, Português (Brasil)
};

class LanguageService : public QObject {
    Q_OBJECT
public:
    explicit LanguageService(QObject* parent = nullptr);

    static QString settingsKey();
    static QString defaultLocaleId();
    static QList<LocaleInfo> supportedLocales();
    static bool isSupported(const QString& localeId);

    QString currentLocaleId() const { return m_currentLocaleId; }

    /// Load translators for localeId. Falls back to English if unavailable.
    /// Returns the locale that was actually applied.
    QString applyLocale(const QString& localeId);

    /// Restore preference from QSettings (or default English).
    QString applySavedPreference();

    void savePreference(const QString& localeId) const;

    void setTranslationsDirectory(const QString& dir);
    QString translationsDirectory() const { return m_translationsDir; }

signals:
    void languageChanged(const QString& localeId);

private:
    void clearTranslators();
    bool installTranslatorFile(const QString& qmBaseName);

    QString m_currentLocaleId;
    QString m_translationsDir;
    std::unique_ptr<QTranslator> m_appTranslator;
    std::unique_ptr<QTranslator> m_qtTranslator;
};

} // namespace ms3d::i18n
