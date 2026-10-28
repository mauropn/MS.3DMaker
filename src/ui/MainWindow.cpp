#include "MainWindow.h"
#include "PreferencesDialog.h"
#include "viewport/OcctView.h"

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
    m_editMenu = menuBar()->addMenu(QString());
    m_viewMenu = menuBar()->addMenu(QString());
    m_helpMenu = menuBar()->addMenu(QString());

    m_createBoxAction = new QAction(this);
    connect(m_createBoxAction, &QAction::triggered, this, &MainWindow::createDemoBox);
    m_fileMenu->addAction(m_createBoxAction);

    m_quitAction = new QAction(this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
    m_fileMenu->addSeparator();
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
    m_editMenu->setTitle(tr("Edit"));
    m_viewMenu->setTitle(tr("View"));
    m_helpMenu->setTitle(tr("Help"));
    m_createBoxAction->setText(tr("New Box"));
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

void MainWindow::createDemoBox()
{
    if (!m_view) {
        return;
    }
    QString error;
    // 100 x 60 x 30 mm demo solid
    if (!m_view->displayBox(100.0, 60.0, 30.0, &error)) {
        QMessageBox::critical(this, tr("Geometry error"),
                              tr("Could not create or display the box.\n%1").arg(error));
        statusBar()->showMessage(tr("Box creation failed"), 5000);
        return;
    }
    statusBar()->showMessage(tr("Displayed box 100 × 60 × 30 mm"), 4000);
}

void MainWindow::fitAllView()
{
    if (m_view) {
        m_view->fitAll();
    }
}

} // namespace ms3d::ui
