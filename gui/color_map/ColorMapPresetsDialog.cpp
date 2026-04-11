#include "ColorMapPresetsDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>

namespace ntcap {

ColorMapPresetsDialog::ColorMapPresetsDialog(QWidget* parent)
    : QDialog(parent)
    , m_presets(ColorMapPresets::all())
{
    setWindowTitle(tr("Select Color Map"));
    setMinimumSize(320, 260);

    auto* root = new QVBoxLayout(this);

    root->addWidget(new QLabel(tr("Available presets:")));

    m_list = new QListWidget;
    for (const auto& preset : m_presets)
        m_list->addItem(preset.name);
    if (!m_presets.isEmpty())
        m_list->setCurrentRow(0);
    root->addWidget(m_list);

    auto* buttons = new QHBoxLayout;
    buttons->addStretch();

    auto* okBtn = new QPushButton(tr("OK"));
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
    buttons->addWidget(okBtn);

    auto* cancelBtn = new QPushButton(tr("Cancel"));
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    buttons->addWidget(cancelBtn);

    root->addLayout(buttons);

    connect(m_list, &QListWidget::itemDoubleClicked,
            this,   &QDialog::accept);
}

ColorMap ColorMapPresetsDialog::selectedColorMap() const
{
    int row = m_list->currentRow();
    if (row < 0 || row >= m_presets.size())
        return {};
    return m_presets[row].map;
}

QString ColorMapPresetsDialog::selectedName() const
{
    int row = m_list->currentRow();
    if (row < 0 || row >= m_presets.size())
        return {};
    return m_presets[row].name;
}

} // namespace ntcap
