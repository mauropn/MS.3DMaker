#pragma once

#include "ai/AiTypes.h"

#include <QDialog>

class QLabel;
class QPlainTextEdit;
class QDialogButtonBox;
class QListWidget;
class QPushButton;

namespace ms3d::ui {

class AiAssistDialog : public QDialog {
    Q_OBJECT
public:
    explicit AiAssistDialog(QWidget* parent = nullptr);

    QString promptText() const;
    const ms3d::ai::AiPlan& plan() const { return m_plan; }
    bool hasValidPlan() const { return m_planReady; }

public slots:
    void generatePlan();

private:
    void retranslateUi();
    void refreshPlanView();

    QLabel* m_promptLabel = nullptr;
    QPlainTextEdit* m_promptEdit = nullptr;
    QLabel* m_planLabel = nullptr;
    QListWidget* m_planList = nullptr;
    QLabel* m_notesLabel = nullptr;
    QPlainTextEdit* m_notesEdit = nullptr;
    QDialogButtonBox* m_buttons = nullptr;
    QPushButton* m_generateButton = nullptr;
    ms3d::ai::AiPlan m_plan;
    bool m_planReady = false;
};

} // namespace ms3d::ui
