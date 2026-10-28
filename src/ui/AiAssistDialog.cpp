#include "AiAssistDialog.h"

#include "ai/AiPlanner.h"
#include "ai/AiValidator.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace ms3d::ui {

AiAssistDialog::AiAssistDialog(QWidget* parent)
    : QDialog(parent)
{
    auto* root = new QVBoxLayout(this);

    m_promptLabel = new QLabel(this);
    m_promptEdit = new QPlainTextEdit(this);
    m_promptEdit->setPlaceholderText(
        QStringLiteral("Create an aluminum enclosure measuring 100 × 60 × 30 mm, with 3 mm walls "
                       "and four mounting holes."));
    m_promptEdit->setMinimumHeight(80);
    root->addWidget(m_promptLabel);
    root->addWidget(m_promptEdit);

    auto* genRow = new QHBoxLayout();
    m_generateButton = new QPushButton(this);
    connect(m_generateButton, &QPushButton::clicked, this, &AiAssistDialog::generatePlan);
    genRow->addStretch(1);
    genRow->addWidget(m_generateButton);
    root->addLayout(genRow);

    m_planLabel = new QLabel(this);
    m_planList = new QListWidget(this);
    m_planList->setMinimumHeight(160);
    root->addWidget(m_planLabel);
    root->addWidget(m_planList);

    m_notesLabel = new QLabel(this);
    m_notesEdit = new QPlainTextEdit(this);
    m_notesEdit->setReadOnly(true);
    m_notesEdit->setMaximumHeight(80);
    root->addWidget(m_notesLabel);
    root->addWidget(m_notesEdit);

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(m_buttons);

    resize(560, 520);
    retranslateUi();
}

void AiAssistDialog::retranslateUi()
{
    setWindowTitle(tr("AI Assist"));
    m_promptLabel->setText(tr("Describe what to create (constrained planner — no arbitrary code):"));
    m_generateButton->setText(tr("Generate plan"));
    m_planLabel->setText(tr("Proposed operations (preview — confirm to execute):"));
    m_notesLabel->setText(tr("Notes"));
    m_buttons->button(QDialogButtonBox::Ok)->setText(tr("Execute plan"));
    m_buttons->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

QString AiAssistDialog::promptText() const
{
    return m_promptEdit->toPlainText();
}

void AiAssistDialog::refreshPlanView()
{
    m_planList->clear();
    for (const auto& step : m_plan.steps) {
        QString line = tr("%1. %2 — %3")
                           .arg(step.id)
                           .arg(QString::fromUtf8(ms3d::ai::aiOpKindName(step.op)))
                           .arg(QString::fromStdString(step.label));
        if (ms3d::ai::isBooleanOp(step.op)) {
            line += tr(" (inputs %1, %2)").arg(step.inputA).arg(step.inputB);
        }
        m_planList->addItem(line);
    }
    QString notes;
    for (const auto& n : m_plan.notes) {
        notes += QString::fromStdString(n) + QLatin1Char('\n');
    }
    m_notesEdit->setPlainText(notes);
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(m_planReady);
}

void AiAssistDialog::generatePlan()
{
    m_planReady = false;
    m_buttons->button(QDialogButtonBox::Ok)->setEnabled(false);

    const auto planned = ms3d::ai::planFromPrompt(promptText().toStdString());
    if (!planned.ok) {
        QMessageBox::warning(this, tr("AI Assist"), QString::fromStdString(planned.error));
        m_plan = {};
        refreshPlanView();
        return;
    }

    const auto validation = ms3d::ai::validatePlan(planned.plan);
    if (!validation.ok) {
        QString details;
        for (const auto& issue : validation.issues) {
            details += QString::fromStdString(issue.message) + QLatin1Char('\n');
        }
        QMessageBox::warning(this, tr("AI Assist"),
                             tr("Plan failed validation:\n%1").arg(details));
        m_plan = {};
        refreshPlanView();
        return;
    }

    m_plan = planned.plan;
    m_planReady = true;
    refreshPlanView();
}

} // namespace ms3d::ui
