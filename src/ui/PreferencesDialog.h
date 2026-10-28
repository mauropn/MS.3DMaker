#pragma once

#include <QDialog>

class QComboBox;
class QDialogButtonBox;
class QLabel;

namespace ms3d::ui {

class PreferencesDialog : public QDialog {
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget* parent = nullptr);

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void apply();

private:
    void retranslateUi();
    void loadLocales();

    QLabel* m_languageLabel = nullptr;
    QComboBox* m_languageCombo = nullptr;
    QDialogButtonBox* m_buttons = nullptr;
};

} // namespace ms3d::ui
