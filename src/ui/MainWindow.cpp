#include "MainWindow.h"
#include "PreferencesDialog.h"

#include <QApplication>
#include <QEvent>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
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
    m_statusLabel = new QLabel(central);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_statusLabel);
    setCentralWidget(central);
    resize(720, 480);

    m_fileMenu = menuBar()->addMenu(QString());
    m_editMenu = menuBar()->addMenu(QString());
    m_viewMenu = menuBar()->addMenu(QString());
    m_helpMenu = menuBar()->addMenu(QString());

    m_preferencesAction = new QAction(this);
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::openPreferences);
    m_editMenu->addAction(m_preferencesAction);

    m_quitAction = new QAction(this);
    connect(m_quitAction, &QAction::triggered, qApp, &QApplication::quit);
    m_fileMenu->addAction(m_quitAction);

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
    m_preferencesAction->setText(tr("Preferences…"));
    m_quitAction->setText(tr("Quit"));
    m_aboutAction->setText(tr("About MS3DMaker"));
    m_statusLabel->setText(tr("MS3DMaker is ready. Open Preferences to change the language."));
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
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

} // namespace ms3d::ui
