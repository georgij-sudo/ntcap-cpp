#include "SeedInputDialog.h"

#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include <limits>

namespace ntcap {

SeedInputDialog::SeedInputDialog(uint32_t currentSeed,
                                 QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Automaton Random Seed"));
    setMinimumWidth(300);

    m_seedSpin = new QSpinBox;
    m_seedSpin->setRange(0, std::numeric_limits<int>::max());
    m_seedSpin->setValue(static_cast<int>(currentSeed & 0x7FFFFFFFu));

    m_copyBtn = new QPushButton(tr("Copy"));
    m_copyBtn->setFixedWidth(60);

    auto* seedRow = new QHBoxLayout;
    seedRow->addWidget(m_seedSpin, 1);
    seedRow->addWidget(m_copyBtn);

    auto* form = new QFormLayout;
    form->addRow(tr("Seed:"), seedRow);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    auto* root = new QVBoxLayout(this);
    root->addLayout(form);
    root->addWidget(buttons);

    connect(m_copyBtn, &QPushButton::clicked,
            this,      &SeedInputDialog::onCopy);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

uint32_t SeedInputDialog::seedValue() const
{
    return static_cast<uint32_t>(m_seedSpin->value());
}

void SeedInputDialog::onCopy()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::number(m_seedSpin->value()));
}

} // namespace ntcap