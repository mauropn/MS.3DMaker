#pragma once

#include "assembly/Assembly.h"
#include "document/Document.h"
#include "drawing/DrawingViews.h"
#include "viewport/OcctView.h"

#include <QMainWindow>

class QAction;
class QListWidget;
class QMenu;

namespace ms3d::ui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

public slots:
    void openPreferences();
    void showAbout();
    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileImportStep();
    void fileExportStep();
    void createDemoBox();
    void createDemoCylinder();
    void createDemoSphere();
    void createDemoExtrusion();
    void createDemoRevolution();
    void createDemoBooleanUnion();
    void createDemoBooleanCut();
    void createDemoBooleanCommon();
    void fitAllView();

    void startSketchSelect();
    void startSketchLine();
    void startSketchCircle();
    void startSketchRectangle();
    void setPlaneXY();
    void setPlaneXZ();
    void setPlaneYZ();
    void applyHorizontalConstraint();
    void applyVerticalConstraint();
    void validateSketch();
    void clearSketch();
    void onSketchStatus(const QString& message);
    void onSketchSelection(int entityId);

    void editSelectedFeature();
    void clearFeatureHistory();
    void onFeatureListSelection();

    void assemblyAddCurrent();
    void assemblyAddOffsetDemo();
    void assemblyShow();
    void assemblyClear();

    void drawingFront();
    void drawingTop();
    void drawingRight();
    void drawingIso();

    void openAiAssist();

protected:
    void changeEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    void retranslateUi();
    void buildUi();
    void updateWindowTitle();
    bool maybeSave();
    void bindDocumentToView();
    void enterSketchTool(ms3d::viewport::SketchTool tool);
    void setSketchPlane(ms3d::sketch::PlaneId plane);
    void refreshFeatureList();
    bool recomputeAndShow(const QString& successMessage);
    int appendPrimitive(ms3d::features::FeatureKind kind,
                        const std::vector<ms3d::features::FeatureParam>& params);
    int appendBooleanDemo(ms3d::features::FeatureKind kind);
    TopoDS_Shape currentSolidSource() const;
    void showDrawingView(ms3d::drawing::ViewOrientation orientation);

    QMenu* m_fileMenu = nullptr;
    QMenu* m_createMenu = nullptr;
    QMenu* m_sketchMenu = nullptr;
    QMenu* m_assemblyMenu = nullptr;
    QMenu* m_drawingMenu = nullptr;
    QMenu* m_toolsMenu = nullptr;
    QMenu* m_editMenu = nullptr;
    QMenu* m_viewMenu = nullptr;
    QMenu* m_helpMenu = nullptr;
    QAction* m_newAction = nullptr;
    QAction* m_openAction = nullptr;
    QAction* m_saveAction = nullptr;
    QAction* m_saveAsAction = nullptr;
    QAction* m_importStepAction = nullptr;
    QAction* m_exportStepAction = nullptr;
    QAction* m_preferencesAction = nullptr;
    QAction* m_quitAction = nullptr;
    QAction* m_aboutAction = nullptr;
    QAction* m_createBoxAction = nullptr;
    QAction* m_createCylinderAction = nullptr;
    QAction* m_createSphereAction = nullptr;
    QAction* m_createExtrusionAction = nullptr;
    QAction* m_createRevolutionAction = nullptr;
    QAction* m_createBoolUnionAction = nullptr;
    QAction* m_createBoolCutAction = nullptr;
    QAction* m_createBoolCommonAction = nullptr;
    QAction* m_fitAllAction = nullptr;
    QAction* m_sketchSelectAction = nullptr;
    QAction* m_sketchLineAction = nullptr;
    QAction* m_sketchCircleAction = nullptr;
    QAction* m_sketchRectAction = nullptr;
    QAction* m_planeXYAction = nullptr;
    QAction* m_planeXZAction = nullptr;
    QAction* m_planeYZAction = nullptr;
    QAction* m_constraintHAction = nullptr;
    QAction* m_constraintVAction = nullptr;
    QAction* m_validateSketchAction = nullptr;
    QAction* m_clearSketchAction = nullptr;
    QAction* m_editFeatureAction = nullptr;
    QAction* m_clearHistoryAction = nullptr;
    QAction* m_assemblyAddAction = nullptr;
    QAction* m_assemblyAddOffsetAction = nullptr;
    QAction* m_assemblyShowAction = nullptr;
    QAction* m_assemblyClearAction = nullptr;
    QAction* m_drawingFrontAction = nullptr;
    QAction* m_drawingTopAction = nullptr;
    QAction* m_drawingRightAction = nullptr;
    QAction* m_drawingIsoAction = nullptr;
    QAction* m_aiAssistAction = nullptr;
    QListWidget* m_featureList = nullptr;
    ms3d::viewport::OcctView* m_view = nullptr;
    TopoDS_Shape shapeForStepExport() const;

    ms3d::document::Document m_document;
    ms3d::assembly::Assembly m_assembly;
    TopoDS_Shape m_importedShape;
    int m_selectedEntityId = -1;
    bool m_demoBoxLoaded = false;
};

} // namespace ms3d::ui
