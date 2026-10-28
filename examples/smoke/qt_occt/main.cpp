#include <QApplication>
#include <QLabel>
#include <QTimer>

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS_Solid.hxx>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QString message;
    try {
        BRepPrimAPI_MakeBox maker(5.0, 5.0, 5.0);
        const TopoDS_Solid solid = maker.Solid();
        BRepCheck_Analyzer analyzer(solid);
        if (!analyzer.IsValid()) {
            message = QObject::tr("Qt+OCCT smoke FAIL: invalid solid");
        } else {
            message = QObject::tr("MS3DMaker Qt+OCCT smoke OK");
        }
    } catch (const Standard_Failure& ex) {
        message = QObject::tr("Qt+OCCT smoke FAIL: %1")
                      .arg(QString::fromUtf8(ex.GetMessageString()));
    } catch (...) {
        message = QObject::tr("Qt+OCCT smoke FAIL: unknown exception");
    }

    QLabel label(message);
    label.setWindowTitle(QObject::tr("MS3DMaker — Qt + OCCT"));
    label.setAlignment(Qt::AlignCenter);
    label.resize(480, 160);
    label.show();

    QTimer::singleShot(1500, &app, &QApplication::quit);
    const int code = app.exec();
    return message.contains(QStringLiteral("OK")) ? code : 1;
}
