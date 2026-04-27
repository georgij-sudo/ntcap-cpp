#include "RecipePresetDialog.h"

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QLabel>

namespace ntcap {

RecipePresetDialog::RecipePresetDialog(const QString& recipeName,
                                       const QList<RecipePreset>& presets,
                                       QWidget* parent)
    : QDialog(parent)
{
    buildUi(recipeName, presets);
}

int RecipePresetDialog::selectedIndex() const noexcept
{
    return m_selected;
}

void RecipePresetDialog::onSelectionChanged(int row)
{
    m_selected = row;
    if (row >= 0 && row < m_descriptions.size())
        m_description->setText(m_descriptions[row]);
    else
        m_description->clear();
}

void RecipePresetDialog::buildUi(const QString& recipeName,
                                  const QList<RecipePreset>& presets)
{
    setWindowTitle(tr("Initialize: %1").arg(recipeName));
    setMinimumSize(520, 340);

    m_list = new QListWidget(this);
    m_list->setMaximumWidth(200);

    m_description = new QTextEdit(this);
    m_description->setReadOnly(true);

    for (const auto& preset : presets) {
        m_list->addItem(preset.name);
        m_descriptions.append(preset.description);
    }

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* leftLayout = new QVBoxLayout;
    leftLayout->addWidget(new QLabel(tr("Presets:"), this));
    leftLayout->addWidget(m_list);

    auto* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(new QLabel(tr("Description:"), this));
    rightLayout->addWidget(m_description);

    auto* bodyLayout = new QHBoxLayout;
    bodyLayout->addLayout(leftLayout, 1);
    bodyLayout->addLayout(rightLayout, 2);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(bodyLayout);
    mainLayout->addWidget(buttons);

    connect(m_list, &QListWidget::currentRowChanged,
            this,   &RecipePresetDialog::onSelectionChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (!presets.isEmpty())
        m_list->setCurrentRow(0);
}

} // namespace ntcap