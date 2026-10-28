#include "MainWindow.h"
#include "FeatureEditDialog.h"
#include "PreferencesDialog.h"
#include "viewport/OcctView.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QDockWidget>
#include <QEvent>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QShowEvent>
#include <QStatusBar>
#include <QVariant>
#include <QVBoxLayout>
#include <QWidget>

namespace ms3d::ui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_sketch(std::make_unique<ms3d::sketch::Sketch>(ms3d::sketch::PlaneId::XY))
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
    resize(1100, 700);

    m_view->setSketch(m_sketch.get());
    connect(m_view, &ms3d::viewport::OcctView::statusMessage, this, &MainWindow::onSketchStatus);
    connect(m_view, &ms3d::viewport::OcctView::sketchSelectionChanged, this,
            &MainWindow::onSketchSelection);

    auto* dock = new QDockWidget(this);
    dock->setObjectName(QStringLiteral("FeatureHistoryDock"));
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_featureList = new QListWidget(dock);
    m_featureList->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(m_featureList, &QListWidget::itemSelectionChanged, this,
            &MainWindow::onFeatureListSelection);
    connect(m_featureList, &QListWidget::itemDoubleClicked, this,
            [this](QListWidgetItem*) { editSelectedFeature(); });
    dock->setWidget(m_featureList);
    addDockWidget(Qt::LeftDockWidgetArea, dock);
    m_featureList->setParent(dock);
    // Keep dock title via retranslate — store as property
    dock->setWindowTitle(tr("Feature History"));
    m_featureList->setProperty("historyDock", QVariant::fromValue(static_cast<QWidget*>(dock)));

    m_fileMenu = menuBar()->addMenu(QString());
    m_createMenu = menuBar()->addMenu(QString());
    m_sketchMenu = menuBar()->addMenu(QString());
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

    m_sketchSelectAction = new QAction(this);
    connect(m_sketchSelectAction, &QAction::triggered, this, &MainWindow::startSketchSelect);
    m_sketchMenu->addAction(m_sketchSelectAction);

    m_sketchLineAction = new QAction(this);
    connect(m_sketchLineAction, &QAction::triggered, this, &MainWindow::startSketchLine);
    m_sketchMenu->addAction(m_sketchLineAction);

    m_sketchCircleAction = new QAction(this);
    connect(m_sketchCircleAction, &QAction::triggered, this, &MainWindow::startSketchCircle);
    m_sketchMenu->addAction(m_sketchCircleAction);

    m_sketchRectAction = new QAction(this);
    connect(m_sketchRectAction, &QAction::triggered, this, &MainWindow::startSketchRectangle);
    m_sketchMenu->addAction(m_sketchRectAction);

    m_sketchMenu->addSeparator();

    m_planeXYAction = new QAction(this);
    connect(m_planeXYAction, &QAction::triggered, this, &MainWindow::setPlaneXY);
    m_sketchMenu->addAction(m_planeXYAction);

    m_planeXZAction = new QAction(this);
    connect(m_planeXZAction, &QAction::triggered, this, &MainWindow::setPlaneXZ);
    m_sketchMenu->addAction(m_planeXZAction);

    m_planeYZAction = new QAction(this);
    connect(m_planeYZAction, &QAction::triggered, this, &MainWindow::setPlaneYZ);
    m_sketchMenu->addAction(m_planeYZAction);

    m_sketchMenu->addSeparator();

    m_constraintHAction = new QAction(this);
    connect(m_constraintHAction, &QAction::triggered, this, &MainWindow::applyHorizontalConstraint);
    m_sketchMenu->addAction(m_constraintHAction);

    m_constraintVAction = new QAction(this);
    connect(m_constraintVAction, &QAction::triggered, this, &MainWindow::applyVerticalConstraint);
    m_sketchMenu->addAction(m_constraintVAction);

    m_validateSketchAction = new QAction(this);
    connect(m_validateSketchAction, &QAction::triggered, this, &MainWindow::validateSketch);
    m_sketchMenu->addAction(m_validateSketchAction);

    m_clearSketchAction = new QAction(this);
    connect(m_clearSketchAction, &QAction::triggered, this, &MainWindow::clearSketch);
    m_sketchMenu->addAction(m_clearSketchAction);

    m_editFeatureAction = new QAction(this);
    connect(m_editFeatureAction, &QAction::triggered, this, &MainWindow::editSelectedFeature);
    m_editMenu->addAction(m_editFeatureAction);

    m_clearHistoryAction = new QAction(this);
    connect(m_clearHistoryAction, &QAction::triggered, this, &MainWindow::clearFeatureHistory);
    m_editMenu->addAction(m_clearHistoryAction);

    m_preferencesAction = new QAction(this);
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::openPreferences);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_preferencesAction);

    m_quitAction = new QAction(this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
    m_fileMenu->addAction(m_quitAction);

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
    m_sketchMenu->setTitle(tr("Sketch"));
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
    m_sketchSelectAction->setText(tr("Select"));
    m_sketchLineAction->setText(tr("Line"));
    m_sketchCircleAction->setText(tr("Circle"));
    m_sketchRectAction->setText(tr("Rectangle"));
    m_planeXYAction->setText(tr("Plane XY"));
    m_planeXZAction->setText(tr("Plane XZ"));
    m_planeYZAction->setText(tr("Plane YZ"));
    m_constraintHAction->setText(tr("Make Horizontal"));
    m_constraintVAction->setText(tr("Make Vertical"));
    m_validateSketchAction->setText(tr("Validate Sketch"));
    m_clearSketchAction->setText(tr("Clear Sketch"));
    m_editFeatureAction->setText(tr("Edit Feature Parameters…"));
    m_clearHistoryAction->setText(tr("Clear Feature History"));
    m_preferencesAction->setText(tr("Preferences…"));
    m_quitAction->setText(tr("Quit"));
    m_fitAllAction->setText(tr("Fit All"));
    m_aboutAction->setText(tr("About MS3DMaker"));
    if (auto* dock = findChild<QDockWidget*>(QStringLiteral("FeatureHistoryDock"))) {
        dock->setWindowTitle(tr("Feature History"));
    }
    statusBar()->showMessage(
        tr("Orbit: Shift+LMB drag · Pan: MMB or Alt+LMB · Zoom: wheel / RMB drag · Select: LMB"));
    refreshFeatureList();
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

void MainWindow::refreshFeatureList()
{
    if (!m_featureList) {
        return;
    }
    const int previousId =
        m_featureList->currentItem() ? m_featureList->currentItem()->data(Qt::UserRole).toInt() : -1;
    m_featureList->clear();
    for (const auto& f : m_history.features()) {
        QString status;
        if (f.valid) {
            status = tr("OK");
        } else if (f.dirty) {
            status = tr("Dirty");
        } else {
            status = tr("Error");
        }
        auto* item = new QListWidgetItem(
            tr("%1 [%2]").arg(QString::fromStdString(f.name), status), m_featureList);
        item->setData(Qt::UserRole, f.id);
        if (!f.error.empty()) {
            item->setToolTip(QString::fromStdString(f.error));
        }
        if (f.id == previousId) {
            m_featureList->setCurrentItem(item);
        }
    }
}

bool MainWindow::recomputeAndShow(const QString& successMessage)
{
    if (!m_view) {
        return false;
    }
    m_view->setSketchTool(ms3d::viewport::SketchTool::None);
    const auto report = m_history.recompute();
    refreshFeatureList();
    if (!report.ok) {
        QMessageBox::critical(this, tr("Feature recompute failed"),
                              QString::fromStdString(report.message));
        statusBar()->showMessage(tr("Feature recompute failed"), 5000);
        return false;
    }
    const auto tip = m_history.tip();
    if (!tip || !tip->valid) {
        statusBar()->showMessage(tr("No valid tip feature to display"), 4000);
        return false;
    }
    QString error;
    if (!m_view->displaySolid(tip->result, &error)) {
        QMessageBox::critical(this, tr("Geometry error"), error);
        return false;
    }
    statusBar()->showMessage(successMessage, 4000);
    return true;
}

int MainWindow::appendPrimitive(ms3d::features::FeatureKind kind,
                                const std::vector<ms3d::features::FeatureParam>& params)
{
    return m_history.addPrimitive(kind, params, ms3d::features::featureKindName(kind));
}

int MainWindow::appendBooleanDemo(ms3d::features::FeatureKind kind)
{
    const int a = m_history.addPrimitive(
        ms3d::features::FeatureKind::Box, {{"dx", 80}, {"dy", 50}, {"dz", 40}}, "Box");
    const int b = m_history.addPrimitive(
        ms3d::features::FeatureKind::Box, {{"dx", 60}, {"dy", 40}, {"dz", 50}}, "Box");
    return m_history.addBoolean(kind, a, b, ms3d::features::featureKindName(kind));
}

void MainWindow::createDemoBox()
{
    appendPrimitive(ms3d::features::FeatureKind::Box, {{"dx", 100}, {"dy", 60}, {"dz", 30}});
    recomputeAndShow(tr("Displayed box feature 100 × 60 × 30 mm"));
}

void MainWindow::createDemoCylinder()
{
    appendPrimitive(ms3d::features::FeatureKind::Cylinder, {{"radius", 25}, {"height", 60}});
    recomputeAndShow(tr("Displayed cylinder feature r=25 h=60 mm"));
}

void MainWindow::createDemoSphere()
{
    appendPrimitive(ms3d::features::FeatureKind::Sphere, {{"radius", 40}});
    recomputeAndShow(tr("Displayed sphere feature r=40 mm"));
}

void MainWindow::createDemoExtrusion()
{
    appendPrimitive(ms3d::features::FeatureKind::Extrusion,
                    {{"width", 80}, {"depth", 50}, {"height", 20}});
    recomputeAndShow(tr("Displayed extrusion feature 80 × 50 × 20 mm"));
}

void MainWindow::createDemoRevolution()
{
    appendPrimitive(ms3d::features::FeatureKind::Revolution,
                    {{"innerRadius", 15}, {"outerRadius", 30}, {"height", 50}});
    recomputeAndShow(tr("Displayed revolution feature (r_in=15 r_out=30 h=50 mm)"));
}

void MainWindow::createDemoBooleanUnion()
{
    appendBooleanDemo(ms3d::features::FeatureKind::BooleanUnion);
    recomputeAndShow(tr("Displayed boolean union feature"));
}

void MainWindow::createDemoBooleanCut()
{
    appendBooleanDemo(ms3d::features::FeatureKind::BooleanCut);
    recomputeAndShow(tr("Displayed boolean subtract feature"));
}

void MainWindow::createDemoBooleanCommon()
{
    appendBooleanDemo(ms3d::features::FeatureKind::BooleanCommon);
    recomputeAndShow(tr("Displayed boolean intersect feature"));
}

void MainWindow::fitAllView()
{
    if (m_view) {
        m_view->fitAll();
    }
}

void MainWindow::enterSketchTool(ms3d::viewport::SketchTool tool)
{
    if (!m_view || !m_sketch) {
        return;
    }
    m_view->setSketch(m_sketch.get());
    m_view->setSketchTool(tool);
    m_view->refreshSketchDisplay();
    m_view->fitAll();
}

void MainWindow::startSketchSelect()
{
    enterSketchTool(ms3d::viewport::SketchTool::Select);
    statusBar()->showMessage(tr("Sketch select: click an entity"), 4000);
}

void MainWindow::startSketchLine()
{
    enterSketchTool(ms3d::viewport::SketchTool::Line);
    statusBar()->showMessage(tr("Sketch line: click two points on the plane"), 5000);
}

void MainWindow::startSketchCircle()
{
    enterSketchTool(ms3d::viewport::SketchTool::Circle);
    statusBar()->showMessage(tr("Sketch circle: center, then radius point"), 5000);
}

void MainWindow::startSketchRectangle()
{
    enterSketchTool(ms3d::viewport::SketchTool::Rectangle);
    statusBar()->showMessage(tr("Sketch rectangle: corner, then opposite corner"), 5000);
}

void MainWindow::setSketchPlane(ms3d::sketch::PlaneId plane)
{
    if (!m_view) {
        return;
    }
    m_view->setSketchPlane(plane);
    statusBar()->showMessage(
        tr("Sketch plane: %1").arg(QString::fromUtf8(ms3d::sketch::planeIdName(plane))), 4000);
}

void MainWindow::setPlaneXY()
{
    setSketchPlane(ms3d::sketch::PlaneId::XY);
}

void MainWindow::setPlaneXZ()
{
    setSketchPlane(ms3d::sketch::PlaneId::XZ);
}

void MainWindow::setPlaneYZ()
{
    setSketchPlane(ms3d::sketch::PlaneId::YZ);
}

void MainWindow::applyHorizontalConstraint()
{
    if (!m_sketch || m_selectedEntityId < 0) {
        statusBar()->showMessage(tr("Select a line first"), 4000);
        return;
    }
    const auto ent = m_sketch->findEntity(m_selectedEntityId);
    if (!ent || ent->kind != ms3d::sketch::EntityKind::Line) {
        statusBar()->showMessage(tr("Horizontal constraint requires a line"), 4000);
        return;
    }
    m_sketch->addConstraint(ms3d::sketch::ConstraintKind::Horizontal, m_selectedEntityId);
    m_sketch->applyConstraints();
    m_view->refreshSketchDisplay();
    statusBar()->showMessage(tr("Applied horizontal constraint"), 4000);
}

void MainWindow::applyVerticalConstraint()
{
    if (!m_sketch || m_selectedEntityId < 0) {
        statusBar()->showMessage(tr("Select a line first"), 4000);
        return;
    }
    const auto ent = m_sketch->findEntity(m_selectedEntityId);
    if (!ent || ent->kind != ms3d::sketch::EntityKind::Line) {
        statusBar()->showMessage(tr("Vertical constraint requires a line"), 4000);
        return;
    }
    m_sketch->addConstraint(ms3d::sketch::ConstraintKind::Vertical, m_selectedEntityId);
    m_sketch->applyConstraints();
    m_view->refreshSketchDisplay();
    statusBar()->showMessage(tr("Applied vertical constraint"), 4000);
}

void MainWindow::validateSketch()
{
    if (!m_sketch) {
        return;
    }
    m_sketch->applyConstraints();
    const auto result = m_sketch->validate();
    if (result.ok) {
        QMessageBox::information(this, tr("Sketch validation"),
                                 tr("Sketch is valid (%1 entities, %2 constraints, %3 dimensions).")
                                     .arg(m_sketch->entities().size())
                                     .arg(m_sketch->constraints().size())
                                     .arg(m_sketch->dimensions().size()));
        statusBar()->showMessage(tr("Sketch valid"), 4000);
        return;
    }
    QString details;
    for (const auto& issue : result.issues) {
        details += QString::fromStdString(issue.message) + QLatin1Char('\n');
    }
    QMessageBox::warning(this, tr("Sketch validation"),
                         tr("Sketch has problems:\n%1").arg(details));
    statusBar()->showMessage(tr("Sketch validation failed"), 5000);
}

void MainWindow::clearSketch()
{
    if (!m_sketch || !m_view) {
        return;
    }
    m_sketch->clear();
    m_selectedEntityId = -1;
    m_view->refreshSketchDisplay();
    statusBar()->showMessage(tr("Sketch cleared"), 3000);
}

void MainWindow::onSketchStatus(const QString& message)
{
    statusBar()->showMessage(message, 4000);
}

void MainWindow::onSketchSelection(int entityId)
{
    m_selectedEntityId = entityId;
    if (entityId < 0) {
        statusBar()->showMessage(tr("No sketch entity selected"), 3000);
        return;
    }
    const auto ent = m_sketch ? m_sketch->findEntity(entityId) : std::nullopt;
    if (!ent) {
        return;
    }
    QString kind;
    switch (ent->kind) {
    case ms3d::sketch::EntityKind::Line:
        kind = tr("Line");
        break;
    case ms3d::sketch::EntityKind::Circle:
        kind = tr("Circle");
        break;
    case ms3d::sketch::EntityKind::Rectangle:
        kind = tr("Rectangle");
        break;
    }
    statusBar()->showMessage(tr("Selected %1 (id %2)").arg(kind).arg(entityId), 4000);
}

void MainWindow::onFeatureListSelection()
{
    if (!m_featureList || !m_featureList->currentItem()) {
        return;
    }
    const int id = m_featureList->currentItem()->data(Qt::UserRole).toInt();
    const auto feature = m_history.find(id);
    if (!feature) {
        return;
    }
    if (feature->valid && m_view) {
        m_view->setSketchTool(ms3d::viewport::SketchTool::None);
        QString error;
        m_view->displaySolid(feature->result, &error);
    }
    statusBar()->showMessage(tr("Feature %1 selected").arg(QString::fromStdString(feature->name)),
                             3000);
}

void MainWindow::editSelectedFeature()
{
    if (!m_featureList || !m_featureList->currentItem()) {
        statusBar()->showMessage(tr("Select a feature in the history list"), 4000);
        return;
    }
    const int id = m_featureList->currentItem()->data(Qt::UserRole).toInt();
    const auto feature = m_history.find(id);
    if (!feature) {
        return;
    }
    if (feature->params.empty()) {
        QMessageBox::information(this, tr("Edit feature"),
                                 tr("This feature has no editable numeric parameters."));
        return;
    }

    FeatureEditDialog dialog(*feature, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }
    std::string err;
    if (!m_history.setParameters(id, dialog.parameters(), &err)) {
        QMessageBox::critical(this, tr("Edit feature"), QString::fromStdString(err));
        return;
    }
    recomputeAndShow(tr("Recomputed after editing %1").arg(QString::fromStdString(feature->name)));
}

void MainWindow::clearFeatureHistory()
{
    m_history.clear();
    refreshFeatureList();
    if (m_view) {
        m_view->clearSolidDisplay();
    }
    statusBar()->showMessage(tr("Feature history cleared"), 3000);
}

} // namespace ms3d::ui
