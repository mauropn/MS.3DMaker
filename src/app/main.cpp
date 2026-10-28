#include "i18n/LanguageService.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QEvent>
#include <QFileInfo>

namespace {

QString resolveTranslationsDir(const QString& appDir) {
    const QStringList candidates = {
        QDir(appDir).filePath(QStringLiteral("translations")),
        QDir(appDir).filePath(QStringLiteral("../translations")),
        QDir(QCoreApplication::applicationDirPath()).filePath(QStringLiteral("translations")),
    };
    for (const QString& candidate : candidates) {
        if (QFileInfo::exists(QDir(candidate).filePath(QStringLiteral("MS3DMaker_en.qm"))) ||
            QFileInfo::exists(QDir(candidate).filePath(QStringLiteral("MS3DMaker_pt_BR.qm")))) {
            return QDir(candidate).absolutePath();
        }
    }
    // Fall back to next to the executable even if not yet present (dev builds).
    return QDir(appDir).filePath(QStringLiteral("translations"));
}

} // namespace

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("MauroSistemas"));
    QApplication::setApplicationName(QStringLiteral("MS3DMaker"));
    QApplication::setApplicationDisplayName(QStringLiteral("MS3DMaker"));

    auto* languages = new ms3d::i18n::LanguageService(&app);
    languages->setTranslationsDirectory(
        resolveTranslationsDir(QCoreApplication::applicationDirPath()));
    languages->applySavedPreference();

    ms3d::ui::MainWindow window;
    QObject::connect(languages, &ms3d::i18n::LanguageService::languageChanged, &window,
                     [&window](const QString&) {
                         // Force a LanguageChange pass for top-level widgets.
                         QEvent event(QEvent::LanguageChange);
                         QCoreApplication::sendEvent(&window, &event);
                     });
    window.show();
    return app.exec();
}
