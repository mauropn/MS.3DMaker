#include "PreferencesDialog.h"

#include "i18n/LanguageService.h"

#include <QAbstractButton>
#include <QApplication>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace ms3d::ui {

namespace {

ms3d::i18n::LanguageService* languageService()
{
    return qApp->findChild<ms3d::i18n::LanguageService*>();
}

} // namespace

PreferencesDialog::PreferencesDialog(QWidget* parent)
    : QDialog(parent)
{
    setModal(true);
    resize(420, 160);

    auto* layout = new QVBoxLayout(this);
    auto* form = new QFormLayout();
    m_languageLabel = new QLabel(this);
    m_languageCombo = new QComboBox(this);
    form->addRow(m_languageLabel, m_languageCombo);
    layout->addLayout(form);

    m_buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply, this);
    connect(m_buttons, &QDialogButtonBox::accepted, this, [this]() {
        apply();
        accept();
    });
    connect(m_buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    if (QAbstractButton* applyButton = m_buttons->button(QDialogButtonBox::Apply)) {
        connect(applyButton, &QAbstractButton::clicked, this, &PreferencesDialog::apply);
    }
    layout->addWidget(m_buttons);

    loadLocales();
    retranslateUi();
}

void PreferencesDialog::loadLocales()
{
    m_languageCombo->clear();
    auto* service = languageService();
    const QString current = service ? service->currentLocaleId()
                                    : ms3d::i18n::LanguageService::defaultLocaleId();

    for (const auto& info : ms3d::i18n::LanguageService::supportedLocales()) {
        m_languageCombo->addItem(info.nativeName, info.id);
        if (info.id == current) {
            m_languageCombo->setCurrentIndex(m_languageCombo->count() - 1);
        }
    }
}

void PreferencesDialog::retranslateUi()
{
    setWindowTitle(tr("Preferences"));
    m_languageLabel->setText(tr("Language"));
    // Native language names stay as stored in LocaleInfo (not translated via tr).
}

void PreferencesDialog::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QDialog::changeEvent(event);
}

void PreferencesDialog::apply()
{
    auto* service = languageService();
    if (!service) {
        return;
    }
    const QString localeId = m_languageCombo->currentData().toString();
    service->savePreference(localeId);
    service->applyLocale(localeId);
}

} // namespace ms3d::ui
