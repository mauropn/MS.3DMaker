#include "FeatureEditDialog.h"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QVBoxLayout>

namespace ms3d::ui {

FeatureEditDialog::FeatureEditDialog(const ms3d::features::Feature& feature, QWidget* parent)
    : QDialog(parent), m_kind(feature.kind) {
    auto* root = new QVBoxLayout(this);
    m_form = new QFormLayout();
    root->addLayout(m_form);

    for (const auto& p : feature.params) {
        auto* spin = new QDoubleSpinBox(this);
        spin->setRange(0.001, 1.0e6);
        spin->setDecimals(3);
        spin->setSuffix(QStringLiteral(" mm"));
        spin->setValue(p.value);
        m_spins.push_back(spin);
        m_keys.push_back(p.key);
        m_form->addRow(QString::fromStdString(p.key), spin);
    }

    m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(m_buttons);

    retranslateUi();
}

void FeatureEditDialog::retranslateUi() {
    setWindowTitle(tr("Edit feature parameters"));
}

std::vector<ms3d::features::FeatureParam> FeatureEditDialog::parameters() const {
    std::vector<ms3d::features::FeatureParam> out;
    out.reserve(m_spins.size());
    for (size_t i = 0; i < m_spins.size(); ++i) {
        out.push_back({m_keys[i], m_spins[i]->value()});
    }
    return out;
}

} // namespace ms3d::ui
