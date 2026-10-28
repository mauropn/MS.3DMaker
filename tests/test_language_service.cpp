#include "i18n/LanguageService.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>
#include <QTemporaryDir>
#include <QtTest/QtTest>

class TestLanguageService : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void defaultLocaleIsEnglish();
    void supportedLocalesIncludePtBr();
    void unknownLocaleFallsBackToEnglish();
    void preferenceRoundTrip();
    void applyEnglishAndPortugueseCatalogs();

private:
    QString m_translationsDir;
};

void TestLanguageService::initTestCase() {
    // Prefer build-dir translations copied next to the test binary.
    const QString beside =
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("translations"));
    if (QFileInfo::exists(QDir(beside).filePath(QStringLiteral("MS3DMaker_en.qm")))) {
        m_translationsDir = beside;
        return;
    }
    // Source-tree relative fallback for developers who ran lrelease manually.
    const QString source =
        QDir(QStringLiteral(MS3D_SOURCE_DIR)).filePath(QStringLiteral("translations"));
    m_translationsDir = source;
}

void TestLanguageService::defaultLocaleIsEnglish() {
    QCOMPARE(ms3d::i18n::LanguageService::defaultLocaleId(), QStringLiteral("en"));
}

void TestLanguageService::supportedLocalesIncludePtBr() {
    const auto locales = ms3d::i18n::LanguageService::supportedLocales();
    QVERIFY(ms3d::i18n::LanguageService::isSupported(QStringLiteral("en")));
    QVERIFY(ms3d::i18n::LanguageService::isSupported(QStringLiteral("pt-BR")));
    QCOMPARE(locales.size(), 2);
    QCOMPARE(locales[1].nativeName, QStringLiteral("Português (Brasil)"));
}

void TestLanguageService::unknownLocaleFallsBackToEnglish() {
    ms3d::i18n::LanguageService service;
    service.setTranslationsDirectory(m_translationsDir);
    QCOMPARE(service.applyLocale(QStringLiteral("xx-ZZ")), QStringLiteral("en"));
}

void TestLanguageService::preferenceRoundTrip() {
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, dir.path());
    QSettings::setDefaultFormat(QSettings::IniFormat);

    ms3d::i18n::LanguageService service;
    service.setTranslationsDirectory(m_translationsDir);
    service.savePreference(QStringLiteral("pt-BR"));

    ms3d::i18n::LanguageService other;
    other.setTranslationsDirectory(m_translationsDir);
    QCOMPARE(other.applySavedPreference(), QStringLiteral("pt-BR"));
}

void TestLanguageService::applyEnglishAndPortugueseCatalogs() {
    if (!QFileInfo::exists(QDir(m_translationsDir).filePath(QStringLiteral("MS3DMaker_en.qm"))) ||
        !QFileInfo::exists(
            QDir(m_translationsDir).filePath(QStringLiteral("MS3DMaker_pt_BR.qm")))) {
        QSKIP("Compiled translation catalogs (.qm) are not available yet.");
    }

    ms3d::i18n::LanguageService service;
    service.setTranslationsDirectory(m_translationsDir);
    QCOMPARE(service.applyLocale(QStringLiteral("en")), QStringLiteral("en"));
    QCOMPARE(service.applyLocale(QStringLiteral("pt-BR")), QStringLiteral("pt-BR"));
}

QTEST_MAIN(TestLanguageService)
#include "test_language_service.moc"
