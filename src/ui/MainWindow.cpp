#include "MainWindow.h"
#include "AiAssistDialog.h"
#include "FeatureEditDialog.h"
#include "PreferencesDialog.h"
#include "ai/AiExecutor.h"
#include "io/StepIO.h"
#include "viewport/OcctView.h"

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <QAbstractItemView>
#include <QApplication>
#include <QCloseEvent>
#include <QDockWidget>
#include <QEvent>
#include <QFileDialog>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QShowEvent>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

namespace ms3d::ui {

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    buildUi();
    retranslateUi();
    bindDocumentToView();
    updateWindowTitle();
}

void MainWindow::buildUi() {
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0);
    m_view = new ms3d::viewport::OcctView(central);
    layout->addWidget(m_view);
    setCentralWidget(central);
    resize(1100, 700);

    connect(m_view, &ms3d::viewport::OcctView::statusMessage, this, &MainWindow::onSketchStatus);
    connect(m_view, &ms3d::viewport::OcctView::sketchSelectionChanged, this,
            &MainWindow::onSketchSelection);
    connect(m_view, &ms3d::viewport::OcctView::sketchChanged, this, [this]() {
        m_document.markDirty();
        updateWindowTitle();
    });

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

    m_fileMenu = menuBar()->addMenu(QString());
    m_createMenu = menuBar()->addMenu(QString());
    m_sketchMenu = menuBar()->addMenu(QString());
    m_assemblyMenu = menuBar()->addMenu(QString());
    m_drawingMenu = menuBar()->addMenu(QString());
    m_toolsMenu = menuBar()->addMenu(QString());
    m_editMenu = menuBar()->addMenu(QString());
    m_viewMenu = menuBar()->addMenu(QString());
    m_helpMenu = menuBar()->addMenu(QString());

    m_newAction = new QAction(this);
    connect(m_newAction, &QAction::triggered, this, &MainWindow::fileNew);
    m_fileMenu->addAction(m_newAction);

    m_openAction = new QAction(this);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::fileOpen);
    m_fileMenu->addAction(m_openAction);

    m_saveAction = new QAction(this);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::fileSave);
    m_fileMenu->addAction(m_saveAction);

    m_saveAsAction = new QAction(this);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::fileSaveAs);
    m_fileMenu->addAction(m_saveAsAction);

    m_fileMenu->addSeparator();

    m_importStepAction = new QAction(this);
    connect(m_importStepAction, &QAction::triggered, this, &MainWindow::fileImportStep);
    m_fileMenu->addAction(m_importStepAction);

    m_exportStepAction = new QAction(this);
    connect(m_exportStepAction, &QAction::triggered, this, &MainWindow::fileExportStep);
    m_fileMenu->addAction(m_exportStepAction);

    m_fileMenu->addSeparator();

    m_quitAction = new QAction(this);
    connect(m_quitAction, &QAction::triggered, this, &QWidget::close);
    m_fileMenu->addAction(m_quitAction);

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
    connect(m_createBoolUnionAction, &QAction::triggered, this,
            &MainWindow::createDemoBooleanUnion);
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

    m_assemblyAddAction = new QAction(this);
    connect(m_assemblyAddAction, &QAction::triggered, this, &MainWindow::assemblyAddCurrent);
    m_assemblyMenu->addAction(m_assemblyAddAction);

    m_assemblyAddOffsetAction = new QAction(this);
    connect(m_assemblyAddOffsetAction, &QAction::triggered, this,
            &MainWindow::assemblyAddOffsetDemo);
    m_assemblyMenu->addAction(m_assemblyAddOffsetAction);

    m_assemblyShowAction = new QAction(this);
    connect(m_assemblyShowAction, &QAction::triggered, this, &MainWindow::assemblyShow);
    m_assemblyMenu->addAction(m_assemblyShowAction);

    m_assemblyClearAction = new QAction(this);
    connect(m_assemblyClearAction, &QAction::triggered, this, &MainWindow::assemblyClear);
    m_assemblyMenu->addAction(m_assemblyClearAction);

    m_drawingFrontAction = new QAction(this);
    connect(m_drawingFrontAction, &QAction::triggered, this, &MainWindow::drawingFront);
    m_drawingMenu->addAction(m_drawingFrontAction);

    m_drawingTopAction = new QAction(this);
    connect(m_drawingTopAction, &QAction::triggered, this, &MainWindow::drawingTop);
    m_drawingMenu->addAction(m_drawingTopAction);

    m_drawingRightAction = new QAction(this);
    connect(m_drawingRightAction, &QAction::triggered, this, &MainWindow::drawingRight);
    m_drawingMenu->addAction(m_drawingRightAction);

    m_drawingIsoAction = new QAction(this);
    connect(m_drawingIsoAction, &QAction::triggered, this, &MainWindow::drawingIso);
    m_drawingMenu->addAction(m_drawingIsoAction);

    m_aiAssistAction = new QAction(this);
    connect(m_aiAssistAction, &QAction::triggered, this, &MainWindow::openAiAssist);
    m_toolsMenu->addAction(m_aiAssistAction);

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

    m_fitAllAction = new QAction(this);
    connect(m_fitAllAction, &QAction::triggered, this, &MainWindow::fitAllView);
    m_viewMenu->addAction(m_fitAllAction);

    m_aboutAction = new QAction(this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    m_helpMenu->addAction(m_aboutAction);
}

void MainWindow::retranslateUi() {
    m_fileMenu->setTitle(tr("File"));
    m_createMenu->setTitle(tr("Create"));
    m_sketchMenu->setTitle(tr("Sketch"));
    m_assemblyMenu->setTitle(tr("Assembly"));
    m_drawingMenu->setTitle(tr("Drawing"));
    m_toolsMenu->setTitle(tr("Tools"));
    m_editMenu->setTitle(tr("Edit"));
    m_viewMenu->setTitle(tr("View"));
    m_helpMenu->setTitle(tr("Help"));
    m_newAction->setText(tr("New"));
    m_openAction->setText(tr("Open…"));
    m_saveAction->setText(tr("Save"));
    m_saveAsAction->setText(tr("Save As…"));
    m_importStepAction->setText(tr("Import STEP…"));
    m_exportStepAction->setText(tr("Export STEP…"));
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
    m_assemblyAddAction->setText(tr("Add Current Solid"));
    m_assemblyAddOffsetAction->setText(tr("Add Current Solid (+X offset)"));
    m_assemblyShowAction->setText(tr("Show Assembly"));
    m_assemblyClearAction->setText(tr("Clear Assembly"));
    m_drawingFrontAction->setText(tr("Front View (HLR)"));
    m_drawingTopAction->setText(tr("Top View (HLR)"));
    m_drawingRightAction->setText(tr("Right View (HLR)"));
    m_drawingIsoAction->setText(tr("Isometric View (HLR)"));
    m_aiAssistAction->setText(tr("AI Assist…"));
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
    updateWindowTitle();
}

void MainWindow::updateWindowTitle() {
    QString title = m_document.title();
    if (m_document.isDirty()) {
        title += QLatin1Char('*');
    }
    setWindowTitle(tr("%1 — MS3DMaker").arg(title));
}

void MainWindow::bindDocumentToView() {
    if (!m_view) {
        return;
    }
    m_view->setSketch(&m_document.sketch());
    m_view->refreshSketchDisplay();
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    if (!m_demoBoxLoaded) {
        createDemoBox();
        m_demoBoxLoaded = true;
    }
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeSave() {
    if (!m_document.isDirty()) {
        return true;
    }
    const auto response = QMessageBox::question(
        this, tr("Unsaved changes"),
        tr("The document has unsaved changes. Save before continuing?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Save);
    if (response == QMessageBox::Cancel) {
        return false;
    }
    if (response == QMessageBox::Discard) {
        return true;
    }
    fileSave();
    return !m_document.isDirty();
}

void MainWindow::fileNew() {
    if (!maybeSave()) {
        return;
    }
    m_document.newDocument();
    m_selectedEntityId = -1;
    bindDocumentToView();
    if (m_view) {
        m_view->clearSolidDisplay();
        m_view->setSketchTool(ms3d::viewport::SketchTool::None);
    }
    refreshFeatureList();
    updateWindowTitle();
    statusBar()->showMessage(tr("New document"), 3000);
}

void MainWindow::fileOpen() {
    if (!maybeSave()) {
        return;
    }
    const QString path =
        QFileDialog::getOpenFileName(this, tr("Open Document"), QString(),
                                     tr("MS3DMaker documents (*.%1);;All files (*.*)")
                                         .arg(QString::fromUtf8(ms3d::document::kFileExtension)));
    if (path.isEmpty()) {
        return;
    }
    const auto loaded = m_document.loadFromFile(path);
    if (loaded.status != ms3d::document::LoadStatus::Ok) {
        QMessageBox::critical(
            this, tr("Open failed"),
            tr("%1\n%2").arg(ms3d::document::Document::statusMessage(loaded.status),
                             loaded.message));
        return;
    }
    m_selectedEntityId = -1;
    bindDocumentToView();
    recomputeAndShow(tr("Opened %1").arg(m_document.title()));
    updateWindowTitle();
}

void MainWindow::fileSave() {
    if (m_document.filePath().isEmpty()) {
        fileSaveAs();
        return;
    }
    const auto saved = m_document.saveToFile(m_document.filePath());
    if (!saved.ok) {
        QMessageBox::critical(this, tr("Save failed"), saved.message);
        return;
    }
    updateWindowTitle();
    statusBar()->showMessage(tr("Saved %1").arg(m_document.filePath()), 4000);
}

void MainWindow::fileSaveAs() {
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Save Document As"), m_document.title() + QStringLiteral(".ms3d"),
        tr("MS3DMaker documents (*.%1)").arg(QString::fromUtf8(ms3d::document::kFileExtension)));
    if (path.isEmpty()) {
        return;
    }
    QString finalPath = path;
    if (!finalPath.endsWith(QStringLiteral(".ms3d"), Qt::CaseInsensitive)) {
        finalPath += QStringLiteral(".ms3d");
    }
    const auto saved = m_document.saveToFile(finalPath);
    if (!saved.ok) {
        QMessageBox::critical(this, tr("Save failed"), saved.message);
        // Never leave a partially applied incompatible overwrite: Document refuses write.
        return;
    }
    updateWindowTitle();
    statusBar()->showMessage(tr("Saved %1").arg(m_document.filePath()), 4000);
}

TopoDS_Shape MainWindow::currentSolidSource() const {
    const auto tip = m_document.history().tip();
    if (tip && tip->valid && !tip->result.IsNull()) {
        return tip->result;
    }
    return m_importedShape;
}

TopoDS_Shape MainWindow::shapeForStepExport() const {
    const auto built = m_assembly.buildCompound();
    if (built.ok && !built.compound.IsNull()) {
        return built.compound;
    }
    return currentSolidSource();
}

void MainWindow::assemblyAddCurrent() {
    const TopoDS_Shape shape = currentSolidSource();
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Assembly"),
                             tr("No solid available. Create a feature or import STEP first."));
        return;
    }
    gp_Trsf identity;
    const int id = m_assembly.addInstance(shape, identity, "Component");
    statusBar()->showMessage(
        tr("Assembly: added component id %1 (total %2)").arg(id).arg(m_assembly.instances().size()),
        4000);
}

void MainWindow::assemblyAddOffsetDemo() {
    const TopoDS_Shape shape = currentSolidSource();
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Assembly"),
                             tr("No solid available. Create a feature or import STEP first."));
        return;
    }
    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(120.0, 0.0, 0.0));
    const int id = m_assembly.addInstance(shape, trsf, "Offset");
    statusBar()->showMessage(tr("Assembly: added offset component id %1 (total %2)")
                                 .arg(id)
                                 .arg(m_assembly.instances().size()),
                             4000);
}

void MainWindow::assemblyShow() {
    const auto built = m_assembly.buildCompound();
    if (!built.ok) {
        QMessageBox::warning(this, tr("Assembly"), QString::fromStdString(built.error));
        return;
    }
    if (!m_view) {
        return;
    }
    m_view->setSketchTool(ms3d::viewport::SketchTool::None);
    QString error;
    if (!m_view->displaySolid(built.compound, &error)) {
        QMessageBox::critical(this, tr("Assembly"), error);
        return;
    }
    statusBar()->showMessage(tr("Showing assembly (%1 components)").arg(built.instanceCount), 4000);
}

void MainWindow::assemblyClear() {
    m_assembly.clear();
    statusBar()->showMessage(tr("Assembly cleared"), 3000);
}

void MainWindow::showDrawingView(ms3d::drawing::ViewOrientation orientation) {
    const TopoDS_Shape shape = currentSolidSource();
    if (shape.IsNull()) {
        QMessageBox::warning(this, tr("Drawing"),
                             tr("No solid available. Create a feature or import STEP first."));
        return;
    }
    const auto view = ms3d::drawing::makeHlrView(shape, orientation);
    if (!view.ok) {
        QMessageBox::critical(this, tr("Drawing"), QString::fromStdString(view.error));
        return;
    }
    if (!m_view) {
        return;
    }
    m_view->setSketchTool(ms3d::viewport::SketchTool::None);
    QString error;
    if (!m_view->displaySolid(view.edges, &error)) {
        QMessageBox::critical(this, tr("Drawing"), error);
        return;
    }
    statusBar()->showMessage(
        tr("Drawing view: %1")
            .arg(QString::fromUtf8(ms3d::drawing::viewOrientationName(orientation))),
        4000);
}

void MainWindow::drawingFront() {
    showDrawingView(ms3d::drawing::ViewOrientation::Front);
}

void MainWindow::drawingTop() {
    showDrawingView(ms3d::drawing::ViewOrientation::Top);
}

void MainWindow::drawingRight() {
    showDrawingView(ms3d::drawing::ViewOrientation::Right);
}

void MainWindow::drawingIso() {
    showDrawingView(ms3d::drawing::ViewOrientation::Iso);
}

void MainWindow::openAiAssist() {
    AiAssistDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted || !dialog.hasValidPlan()) {
        return;
    }

    const auto confirm =
        QMessageBox::question(this, tr("Confirm AI plan"),
                              tr("Execute %1 validated modeling step(s) through Feature History?")
                                  .arg(dialog.plan().steps.size()),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (confirm != QMessageBox::Yes) {
        return;
    }

    const auto executed = ms3d::ai::executePlan(m_document.history(), dialog.plan());
    refreshFeatureList();
    if (!executed.ok) {
        QMessageBox::critical(this, tr("AI Assist"), QString::fromStdString(executed.message));
        statusBar()->showMessage(tr("AI plan execution failed"), 5000);
        return;
    }

    m_document.markDirty();
    updateWindowTitle();
    recomputeAndShow(tr("AI plan executed (%1 features)").arg(executed.createdFeatureIds.size()));
}

void MainWindow::fileImportStep() {
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Import STEP"), QString(), tr("STEP files (*.step *.stp);;All files (*.*)"));
    if (path.isEmpty() || !m_view) {
        return;
    }

    const auto imported = ms3d::io::importStep(path.toStdString());
    if (!imported.ok) {
        QMessageBox::critical(this, tr("STEP import failed"),
                              QString::fromStdString(imported.error));
        return;
    }

    m_importedShape = imported.shape;
    m_view->setSketchTool(ms3d::viewport::SketchTool::None);
    QString error;
    if (!m_view->displaySolid(m_importedShape, &error)) {
        QMessageBox::critical(this, tr("STEP import failed"), error);
        return;
    }
    statusBar()->showMessage(
        tr("Imported STEP (%1 root(s)): %2").arg(imported.transferredRoots).arg(path), 5000);
}

void MainWindow::fileExportStep() {
    const TopoDS_Shape shape = shapeForStepExport();
    if (shape.IsNull()) {
        QMessageBox::warning(
            this, tr("STEP export"),
            tr("Nothing to export. Create a feature or import a STEP file first."));
        return;
    }

    const QString path = QFileDialog::getSaveFileName(this, tr("Export STEP"),
                                                      m_document.title() + QStringLiteral(".step"),
                                                      tr("STEP files (*.step *.stp)"));
    if (path.isEmpty()) {
        return;
    }
    QString finalPath = path;
    if (!finalPath.endsWith(QStringLiteral(".step"), Qt::CaseInsensitive) &&
        !finalPath.endsWith(QStringLiteral(".stp"), Qt::CaseInsensitive)) {
        finalPath += QStringLiteral(".step");
    }

    const auto exported =
        ms3d::io::exportStep(shape, finalPath.toStdString(), ms3d::io::StepSchema::AP214);
    if (!exported.ok) {
        QMessageBox::critical(this, tr("STEP export failed"),
                              QString::fromStdString(exported.error));
        return;
    }
    statusBar()->showMessage(tr("Exported STEP (AP214): %1").arg(finalPath), 5000);
}

void MainWindow::openPreferences() {
    PreferencesDialog dialog(this);
    dialog.exec();
}

void MainWindow::showAbout() {
    QMessageBox::about(
        this, tr("About MS3DMaker"),
        tr("MS3DMaker is an independent open-source parametric 3D CAD application."));
}

void MainWindow::refreshFeatureList() {
    if (!m_featureList) {
        return;
    }
    const int previousId = m_featureList->currentItem()
                               ? m_featureList->currentItem()->data(Qt::UserRole).toInt()
                               : -1;
    m_featureList->clear();
    for (const auto& f : m_document.history().features()) {
        QString status;
        if (f.valid) {
            status = tr("OK");
        } else if (f.dirty) {
            status = tr("Dirty");
        } else {
            status = tr("Error");
        }
        auto* item = new QListWidgetItem(tr("%1 [%2]").arg(QString::fromStdString(f.name), status),
                                         m_featureList);
        item->setData(Qt::UserRole, f.id);
        if (!f.error.empty()) {
            item->setToolTip(QString::fromStdString(f.error));
        }
        if (f.id == previousId) {
            m_featureList->setCurrentItem(item);
        }
    }
}

bool MainWindow::recomputeAndShow(const QString& successMessage) {
    if (!m_view) {
        return false;
    }
    m_view->setSketchTool(ms3d::viewport::SketchTool::None);
    const auto report = m_document.history().recompute();
    refreshFeatureList();
    if (!report.ok) {
        QMessageBox::critical(this, tr("Feature recompute failed"),
                              QString::fromStdString(report.message));
        statusBar()->showMessage(tr("Feature recompute failed"), 5000);
        return false;
    }
    const auto tip = m_document.history().tip();
    if (!tip || !tip->valid) {
        if (m_document.history().features().empty()) {
            m_view->clearSolidDisplay();
            statusBar()->showMessage(successMessage, 4000);
            return true;
        }
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
                                const std::vector<ms3d::features::FeatureParam>& params) {
    return m_document.history().addPrimitive(kind, params, ms3d::features::featureKindName(kind));
}

int MainWindow::appendBooleanDemo(ms3d::features::FeatureKind kind) {
    const int a = m_document.history().addPrimitive(ms3d::features::FeatureKind::Box,
                                                    {{"dx", 80}, {"dy", 50}, {"dz", 40}}, "Box");
    const int b = m_document.history().addPrimitive(ms3d::features::FeatureKind::Box,
                                                    {{"dx", 60}, {"dy", 40}, {"dz", 50}}, "Box");
    return m_document.history().addBoolean(kind, a, b, ms3d::features::featureKindName(kind));
}

void MainWindow::createDemoBox() {
    appendPrimitive(ms3d::features::FeatureKind::Box, {{"dx", 100}, {"dy", 60}, {"dz", 30}});
    recomputeAndShow(tr("Displayed box feature 100 × 60 × 30 mm"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::createDemoCylinder() {
    appendPrimitive(ms3d::features::FeatureKind::Cylinder, {{"radius", 25}, {"height", 60}});
    recomputeAndShow(tr("Displayed cylinder feature r=25 h=60 mm"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::createDemoSphere() {
    appendPrimitive(ms3d::features::FeatureKind::Sphere, {{"radius", 40}});
    recomputeAndShow(tr("Displayed sphere feature r=40 mm"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::createDemoExtrusion() {
    appendPrimitive(ms3d::features::FeatureKind::Extrusion,
                    {{"width", 80}, {"depth", 50}, {"height", 20}});
    recomputeAndShow(tr("Displayed extrusion feature 80 × 50 × 20 mm"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::createDemoRevolution() {
    appendPrimitive(ms3d::features::FeatureKind::Revolution,
                    {{"innerRadius", 15}, {"outerRadius", 30}, {"height", 50}});
    recomputeAndShow(tr("Displayed revolution feature (r_in=15 r_out=30 h=50 mm)"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::createDemoBooleanUnion() {
    appendBooleanDemo(ms3d::features::FeatureKind::BooleanUnion);
    recomputeAndShow(tr("Displayed boolean union feature"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::createDemoBooleanCut() {
    appendBooleanDemo(ms3d::features::FeatureKind::BooleanCut);
    recomputeAndShow(tr("Displayed boolean subtract feature"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::createDemoBooleanCommon() {
    appendBooleanDemo(ms3d::features::FeatureKind::BooleanCommon);
    recomputeAndShow(tr("Displayed boolean intersect feature"));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::fitAllView() {
    if (m_view) {
        m_view->fitAll();
    }
}

void MainWindow::enterSketchTool(ms3d::viewport::SketchTool tool) {
    if (!m_view) {
        return;
    }
    m_view->setSketch(&m_document.sketch());
    m_view->setSketchTool(tool);
    m_view->refreshSketchDisplay();
    m_view->fitAll();
}

void MainWindow::startSketchSelect() {
    enterSketchTool(ms3d::viewport::SketchTool::Select);
    statusBar()->showMessage(tr("Sketch select: click an entity"), 4000);
}

void MainWindow::startSketchLine() {
    enterSketchTool(ms3d::viewport::SketchTool::Line);
    statusBar()->showMessage(tr("Sketch line: click two points on the plane"), 5000);
}

void MainWindow::startSketchCircle() {
    enterSketchTool(ms3d::viewport::SketchTool::Circle);
    statusBar()->showMessage(tr("Sketch circle: center, then radius point"), 5000);
}

void MainWindow::startSketchRectangle() {
    enterSketchTool(ms3d::viewport::SketchTool::Rectangle);
    statusBar()->showMessage(tr("Sketch rectangle: corner, then opposite corner"), 5000);
}

void MainWindow::setSketchPlane(ms3d::sketch::PlaneId plane) {
    if (!m_view) {
        return;
    }
    m_view->setSketchPlane(plane);
    m_document.markDirty();
    updateWindowTitle();
    statusBar()->showMessage(
        tr("Sketch plane: %1").arg(QString::fromUtf8(ms3d::sketch::planeIdName(plane))), 4000);
}

void MainWindow::setPlaneXY() {
    setSketchPlane(ms3d::sketch::PlaneId::XY);
}

void MainWindow::setPlaneXZ() {
    setSketchPlane(ms3d::sketch::PlaneId::XZ);
}

void MainWindow::setPlaneYZ() {
    setSketchPlane(ms3d::sketch::PlaneId::YZ);
}

void MainWindow::applyHorizontalConstraint() {
    auto& sketch = m_document.sketch();
    if (m_selectedEntityId < 0) {
        statusBar()->showMessage(tr("Select a line first"), 4000);
        return;
    }
    const auto ent = sketch.findEntity(m_selectedEntityId);
    if (!ent || ent->kind != ms3d::sketch::EntityKind::Line) {
        statusBar()->showMessage(tr("Horizontal constraint requires a line"), 4000);
        return;
    }
    sketch.addConstraint(ms3d::sketch::ConstraintKind::Horizontal, m_selectedEntityId);
    sketch.applyConstraints();
    m_view->refreshSketchDisplay();
    m_document.markDirty();
    updateWindowTitle();
    statusBar()->showMessage(tr("Applied horizontal constraint"), 4000);
}

void MainWindow::applyVerticalConstraint() {
    auto& sketch = m_document.sketch();
    if (m_selectedEntityId < 0) {
        statusBar()->showMessage(tr("Select a line first"), 4000);
        return;
    }
    const auto ent = sketch.findEntity(m_selectedEntityId);
    if (!ent || ent->kind != ms3d::sketch::EntityKind::Line) {
        statusBar()->showMessage(tr("Vertical constraint requires a line"), 4000);
        return;
    }
    sketch.addConstraint(ms3d::sketch::ConstraintKind::Vertical, m_selectedEntityId);
    sketch.applyConstraints();
    m_view->refreshSketchDisplay();
    m_document.markDirty();
    updateWindowTitle();
    statusBar()->showMessage(tr("Applied vertical constraint"), 4000);
}

void MainWindow::validateSketch() {
    auto& sketch = m_document.sketch();
    sketch.applyConstraints();
    const auto result = sketch.validate();
    if (result.ok) {
        QMessageBox::information(this, tr("Sketch validation"),
                                 tr("Sketch is valid (%1 entities, %2 constraints, %3 dimensions).")
                                     .arg(sketch.entities().size())
                                     .arg(sketch.constraints().size())
                                     .arg(sketch.dimensions().size()));
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

void MainWindow::clearSketch() {
    m_document.sketch().clear();
    m_selectedEntityId = -1;
    if (m_view) {
        m_view->refreshSketchDisplay();
    }
    m_document.markDirty();
    updateWindowTitle();
    statusBar()->showMessage(tr("Sketch cleared"), 3000);
}

void MainWindow::onSketchStatus(const QString& message) {
    statusBar()->showMessage(message, 4000);
}

void MainWindow::onSketchSelection(int entityId) {
    m_selectedEntityId = entityId;
    if (entityId < 0) {
        statusBar()->showMessage(tr("No sketch entity selected"), 3000);
        return;
    }
    const auto ent = m_document.sketch().findEntity(entityId);
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

void MainWindow::onFeatureListSelection() {
    if (!m_featureList || !m_featureList->currentItem()) {
        return;
    }
    const int id = m_featureList->currentItem()->data(Qt::UserRole).toInt();
    const auto feature = m_document.history().find(id);
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

void MainWindow::editSelectedFeature() {
    if (!m_featureList || !m_featureList->currentItem()) {
        statusBar()->showMessage(tr("Select a feature in the history list"), 4000);
        return;
    }
    const int id = m_featureList->currentItem()->data(Qt::UserRole).toInt();
    const auto feature = m_document.history().find(id);
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
    if (!m_document.history().setParameters(id, dialog.parameters(), &err)) {
        QMessageBox::critical(this, tr("Edit feature"), QString::fromStdString(err));
        return;
    }
    recomputeAndShow(tr("Recomputed after editing %1").arg(QString::fromStdString(feature->name)));
    m_document.markDirty();
    updateWindowTitle();
}

void MainWindow::clearFeatureHistory() {
    m_document.history().clear();
    refreshFeatureList();
    if (m_view) {
        m_view->clearSolidDisplay();
    }
    m_document.markDirty();
    updateWindowTitle();
    statusBar()->showMessage(tr("Feature history cleared"), 3000);
}

} // namespace ms3d::ui
