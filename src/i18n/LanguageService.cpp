#include "LanguageService.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QSettings>

namespace ms3d::i18n {

LanguageService::LanguageService(QObject* parent)
    : QObject(parent), m_currentLocaleId(defaultLocaleId()) {
}

QString LanguageService::settingsKey() {
    return QStringLiteral("ui/language");
}

QString LanguageService::defaultLocaleId() {
    return QStringLiteral("en");
}

QList<LocaleInfo> LanguageService::supportedLocales() {
    return {
        {QStringLiteral("en"), QStringLiteral("English")},
        {QStringLiteral("pt-BR"), QStringLiteral("Português (Brasil)")},
    };
}

bool LanguageService::isSupported(const QString& localeId) {
    for (const auto& info : supportedLocales()) {
        if (info.id == localeId) {
            return true;
        }
    }
    return false;
}

void LanguageService::setTranslationsDirectory(const QString& dir) {
    m_translationsDir = dir;
}

void LanguageService::clearTranslators() {
    auto* app = QCoreApplication::instance();
    if (!app) {
        return;
    }
    if (m_appTranslator) {
        app->removeTranslator(m_appTranslator.get());
        m_appTranslator.reset();
    }
    if (m_qtTranslator) {
        app->removeTranslator(m_qtTranslator.get());
        m_qtTranslator.reset();
    }
}

bool LanguageService::installTranslatorFile(const QString& qmBaseName) {
    auto translator = std::make_unique<QTranslator>();
    const QString path = QDir(m_translationsDir).filePath(qmBaseName + QStringLiteral(".qm"));
    if (!QFileInfo::exists(path)) {
        return false;
    }
    if (!translator->load(path)) {
        return false;
    }
    QCoreApplication::installTranslator(translator.get());
    m_appTranslator = std::move(translator);
    return true;
}

QString LanguageService::applyLocale(const QString& localeId) {
    QString requested = localeId;
    if (!isSupported(requested)) {
        requested = defaultLocaleId();
    }

    clearTranslators();

    // Catalog file names use underscore: MS3DMaker_pt_BR.qm
    QString fileLocale = requested;
    fileLocale.replace(QLatin1Char('-'), QLatin1Char('_'));
    const QString qmName = QStringLiteral("MS3DMaker_") + fileLocale;

    bool loaded = installTranslatorFile(qmName);
    if (!loaded && requested != defaultLocaleId()) {
        // Safe fallback to English
        requested = defaultLocaleId();
        fileLocale = requested;
        fileLocale.replace(QLatin1Char('-'), QLatin1Char('_'));
        loaded = installTranslatorFile(QStringLiteral("MS3DMaker_") + fileLocale);
    }

    // Optional Qt base translations (menus of standard dialogs, etc.)
    const QString qtLocale =
        (requested == QStringLiteral("pt-BR")) ? QStringLiteral("pt_BR") : QStringLiteral("en");
    auto qtTr = std::make_unique<QTranslator>();
    if (qtTr->load(QStringLiteral("qtbase_") + qtLocale,
                   QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        QCoreApplication::installTranslator(qtTr.get());
        m_qtTranslator = std::move(qtTr);
    }

    const bool changed = (m_currentLocaleId != requested);
    m_currentLocaleId = requested;
    if (changed) {
        emit languageChanged(m_currentLocaleId);
    }
    return m_currentLocaleId;
}

QString LanguageService::applySavedPreference() {
    QSettings settings;
    const QString saved = settings.value(settingsKey(), defaultLocaleId()).toString();
    return applyLocale(saved);
}

void LanguageService::savePreference(const QString& localeId) const {
    QString id = localeId;
    if (!isSupported(id)) {
        id = defaultLocaleId();
    }
    QSettings settings;
    settings.setValue(settingsKey(), id);
}

} // namespace ms3d::i18n
