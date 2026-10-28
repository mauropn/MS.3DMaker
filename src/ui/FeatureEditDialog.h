#pragma once

#include "features/FeatureTypes.h"

#include <QDialog>

#include <vector>

class QDoubleSpinBox;
class QFormLayout;
class QDialogButtonBox;

namespace ms3d::ui {

class FeatureEditDialog : public QDialog {
    Q_OBJECT
public:
    explicit FeatureEditDialog(const ms3d::features::Feature& feature, QWidget* parent = nullptr);

    std::vector<ms3d::features::FeatureParam> parameters() const;

private:
    void retranslateUi();
    ms3d::features::FeatureKind m_kind;
    QFormLayout* m_form = nullptr;
    QDialogButtonBox* m_buttons = nullptr;
    std::vector<QDoubleSpinBox*> m_spins;
    std::vector<std::string> m_keys;
};

} // namespace ms3d::ui
