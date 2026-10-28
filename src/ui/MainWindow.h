#pragma once

#include <QMainWindow>

class QAction;
class QMenu;

namespace ms3d::viewport {
class OcctView;
}

namespace ms3d::ui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

public slots:
    void openPreferences();
    void showAbout();
    void createDemoBox();
    void fitAllView();

protected:
    void changeEvent(QEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void retranslateUi();
    void buildUi();

    QMenu* m_fileMenu = nullptr;
    QMenu* m_editMenu = nullptr;
    QMenu* m_viewMenu = nullptr;
    QMenu* m_helpMenu = nullptr;
    QAction* m_preferencesAction = nullptr;
    QAction* m_quitAction = nullptr;
    QAction* m_aboutAction = nullptr;
    QAction* m_createBoxAction = nullptr;
    QAction* m_fitAllAction = nullptr;
    ms3d::viewport::OcctView* m_view = nullptr;
    bool m_demoBoxLoaded = false;
};

} // namespace ms3d::ui
