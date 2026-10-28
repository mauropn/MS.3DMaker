#pragma once

#include <QMainWindow>

class QAction;
class QLabel;
class QMenu;

namespace ms3d::ui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

public slots:
    void openPreferences();
    void showAbout();

protected:
    void changeEvent(QEvent* event) override;

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
    QLabel* m_statusLabel = nullptr;
};

} // namespace ms3d::ui
