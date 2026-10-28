#include <QApplication>
#include <QLabel>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QLabel label(QObject::tr("MS3DMaker Qt smoke OK"));
    label.setWindowTitle(QObject::tr("MS3DMaker — Qt Hello"));
    label.setAlignment(Qt::AlignCenter);
    label.resize(420, 160);
    label.show();

    // Auto-close so the smoke test can run unattended.
    QTimer::singleShot(1500, &app, &QApplication::quit);
    return app.exec();
}
