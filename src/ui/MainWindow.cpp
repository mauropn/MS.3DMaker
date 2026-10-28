#include "MainWindow.h"
#include "PreferencesDialog.h"
#include "geometry/SolidFactory.h"
#include "viewport/OcctView.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <QApplication>
#include <QEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QShowEvent>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

namespace ms3d::ui {
namespace {

TopoDS_Solid translatedBox(double dx, double dy, double dz, double tx, double ty, double tz)
{
    BRepPrimAPI_MakeBox maker(dx, dy, dz);
    maker.Build();
    TopoDS_Solid solid = maker.Solid();
    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(tx, ty, tz));
    BRepBuilderAPI_Transform xform(solid, trsf);
    return TopoDS::Solid(xform.Shape());
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    buildUi();
    retranslateUi();
}

void MainWindow::buildUi()
{
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    m_view = new ms3d::viewport::OcctView(central);
    layout->addWidget(m_view);
    setCentralWidget(central);
    resize(960, 640);

    m_fileMenu = menuBar()->addMenu(QString());
    m_createMenu = menuBar()->addMenu(QString());
    m_editMenu = menuBar()->addMenu(QString());
    m_viewMenu = menuBar()->addMenu(QString());
    m_helpMenu = menuBar()->addMenu(QString());

    m_createBoxAction = new QAction(this);
    connect(m_createBoxAction, &QAction::triggered, this, &MainWindow::createDemoBox);
    m_createMenu->addAction(m_createBoxAction);

    m_createCylinderAction = new QAction(this);
    connect(m_createCylinderAction, &QAction::triggered, this, &MainWindow::createDemoCylinder);
    m_createMenu->addAction(m_createCylinderAction);

    m_createSphereAction = new QAction(this);
    connect(m_createSphereAction, &QAction::triggered, this, &MainWindow::createDemoSphere);
    m_createMenu->addAction(m_createSphereAction);

    m_createExtrusionAction = new QAction(this);
    connect(m_createExtrusionAction, &QAction::triggered, this, &MainWindow::createDemoExtrusion);
    m_createMenu->addAction(m_createExtrusionAction);

    m_createRevolutionAction = new QAction(this);
    connect(m_createRevolutionAction, &QAction::triggered, this, &MainWindow::createDemoRevolution);
    m_createMenu->addAction(m_createRevolutionAction);

    m_createMenu->addSeparator();

    m_createBoolUnionAction = new QAction(this);
    connect(m_createBoolUnionAction, &QAction::triggered, this, &MainWindow::createDemoBooleanUnion);
    m_createMenu->addAction(m_createBoolUnionAction);

    m_createBoolCutAction = new QAction(this);
    connect(m_createBoolCutAction, &QAction::triggered, this, &MainWindow::createDemoBooleanCut);
    m_createMenu->addAction(m_createBoolCutAction);

    m_createBoolCommonAction = new QAction(this);
    connect(m_createBoolCommonAction, &QAction::triggered, this,
            &MainWindow::createDemoBooleanCommon);
    m_createMenu->addAction(m_createBoolCommonAction);

    m_quitAction = new QAction(this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
    m_fileMenu->addAction(m_quitAction);

    m_preferencesAction = new QAction(this);
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::openPreferences);
    m_editMenu->addAction(m_preferencesAction);

    m_fitAllAction = new QAction(this);
    connect(m_fitAllAction, &QAction::triggered, this, &MainWindow::fitAllView);
    m_viewMenu->addAction(m_fitAllAction);

    m_aboutAction = new QAction(this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tr("MS3DMaker"));
    m_fileMenu->setTitle(tr("File"));
    m_createMenu->setTitle(tr("Create"));
    m_editMenu->setTitle(tr("Edit"));
    m_viewMenu->setTitle(tr("View"));
    m_helpMenu->setTitle(tr("Help"));
    m_createBoxAction->setText(tr("Box"));
    m_createCylinderAction->setText(tr("Cylinder"));
    m_createSphereAction->setText(tr("Sphere"));
    m_createExtrusionAction->setText(tr("Extrusion"));
    m_createRevolutionAction->setText(tr("Revolution"));
    m_createBoolUnionAction->setText(tr("Boolean Union"));
    m_createBoolCutAction->setText(tr("Boolean Subtract"));
    m_createBoolCommonAction->setText(tr("Boolean Intersect"));
    m_preferencesAction->setText(tr("Preferences…"));
    m_quitAction->setText(tr("Quit"));
    m_fitAllAction->setText(tr("Fit All"));
    m_aboutAction->setText(tr("About MS3DMaker"));
    statusBar()->showMessage(
        tr("Orbit: Shift+LMB drag · Pan: MMB or Alt+LMB · Zoom: wheel / RMB drag · Select: LMB"));
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    if (!m_demoBoxLoaded) {
        createDemoBox();
        m_demoBoxLoaded = true;
    }
}

void MainWindow::openPreferences()
{
    PreferencesDialog dialog(this);
    dialog.exec();
}

void MainWindow::showAbout()
{
    QMessageBox::about(
        this,
        tr("About MS3DMaker"),
        tr("MS3DMaker is an independent open-source parametric 3D CAD application."));
}

bool MainWindow::showSolidResult(bool ok, const QString& error, const QString& successMessage,
                                 const QString& failMessage)
{
    if (!ok) {
        QMessageBox::critical(this, tr("Geometry error"),
                              tr("%1\n%2").arg(failMessage, error));
        statusBar()->showMessage(failMessage, 5000);
        return false;
    }
    statusBar()->showMessage(successMessage, 4000);
    return true;
}

void MainWindow::createDemoBox()
{
    if (!m_view) {
        return;
    }
    QString error;
    const bool ok = m_view->displayBox(100.0, 60.0, 30.0, &error);
    showSolidResult(ok, error, tr("Displayed box 100 × 60 × 30 mm"),
                    tr("Could not create or display the box."));
}

void MainWindow::createDemoCylinder()
{
    if (!m_view) {
        return;
    }
    const auto created = ms3d::geometry::makeCylinder(25.0, 60.0);
    QString error = QString::fromStdString(created.error);
    const bool ok = created.ok && m_view->displaySolid(created.solid, &error);
    showSolidResult(ok, error, tr("Displayed cylinder r=25 h=60 mm"),
                    tr("Could not create or display the cylinder."));
}

void MainWindow::createDemoSphere()
{
    if (!m_view) {
        return;
    }
    const auto created = ms3d::geometry::makeSphere(40.0);
    QString error = QString::fromStdString(created.error);
    const bool ok = created.ok && m_view->displaySolid(created.solid, &error);
    showSolidResult(ok, error, tr("Displayed sphere r=40 mm"),
                    tr("Could not create or display the sphere."));
}

void MainWindow::createDemoExtrusion()
{
    if (!m_view) {
        return;
    }
    const auto created = ms3d::geometry::makeExtrusion(80.0, 50.0, 20.0);
    QString error = QString::fromStdString(created.error);
    const bool ok = created.ok && m_view->displaySolid(created.solid, &error);
    showSolidResult(ok, error, tr("Displayed extrusion 80 × 50 × 20 mm"),
                    tr("Could not create or display the extrusion."));
}

void MainWindow::createDemoRevolution()
{
    if (!m_view) {
        return;
    }
    const auto created = ms3d::geometry::makeRevolution(15.0, 30.0, 50.0);
    QString error = QString::fromStdString(created.error);
    const bool ok = created.ok && m_view->displaySolid(created.solid, &error);
    showSolidResult(ok, error, tr("Displayed revolution tube (r_in=15 r_out=30 h=50 mm)"),
                    tr("Could not create or display the revolution."));
}

void MainWindow::createDemoBooleanUnion()
{
    if (!m_view) {
        return;
    }
    const auto a = ms3d::geometry::makeBox(80.0, 50.0, 40.0);
    const TopoDS_Solid b = translatedBox(60.0, 40.0, 50.0, 30.0, 15.0, 10.0);
    const auto created = ms3d::geometry::booleanUnion(a.solid, b);
    QString error = QString::fromStdString(created.error);
    const bool ok = a.ok && created.ok && m_view->displaySolid(created.solid, &error);
    showSolidResult(ok, error, tr("Displayed boolean union of two boxes"),
                    tr("Could not create or display the boolean union."));
}

void MainWindow::createDemoBooleanCut()
{
    if (!m_view) {
        return;
    }
    const auto a = ms3d::geometry::makeBox(80.0, 50.0, 40.0);
    const TopoDS_Solid b = translatedBox(60.0, 40.0, 50.0, 30.0, 15.0, 10.0);
    const auto created = ms3d::geometry::booleanCut(a.solid, b);
    QString error = QString::fromStdString(created.error);
    const bool ok = a.ok && created.ok && m_view->displaySolid(created.solid, &error);
    showSolidResult(ok, error, tr("Displayed boolean subtract of two boxes"),
                    tr("Could not create or display the boolean subtract."));
}

void MainWindow::createDemoBooleanCommon()
{
    if (!m_view) {
        return;
    }
    const auto a = ms3d::geometry::makeBox(80.0, 50.0, 40.0);
    const TopoDS_Solid b = translatedBox(60.0, 40.0, 50.0, 30.0, 15.0, 10.0);
    const auto created = ms3d::geometry::booleanCommon(a.solid, b);
    QString error = QString::fromStdString(created.error);
    const bool ok = a.ok && created.ok && m_view->displaySolid(created.solid, &error);
    showSolidResult(ok, error, tr("Displayed boolean intersect of two boxes"),
                    tr("Could not create or display the boolean intersect."));
}

void MainWindow::fitAllView()
{
    if (m_view) {
        m_view->fitAll();
    }
}

} // namespace ms3d::ui
