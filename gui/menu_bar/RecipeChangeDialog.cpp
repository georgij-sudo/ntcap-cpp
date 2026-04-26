#include "RecipeChangeDialog.h"
#include "RecipeRegistry.h"
#include "RecipeParameterWidget.h"

#include <QListWidget>
#include <QStackedWidget>
#include <QSpinBox>
#include <QLabel>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

namespace ntcap {

RecipeChangeDialog::RecipeChangeDialog(size_t currentWidth,
                                       size_t currentHeight,
                                       size_t currentPlanes,
                                       QWidget* parent)
    : QDialog(parent)
    , m_descriptors(RecipeRegistry::builtIn())
    , m_currentWidth(currentWidth)
    , m_currentHeight(currentHeight)
    , m_currentPlanes(currentPlanes)
{
    setWindowTitle(tr("Change Recipe"));
    setMinimumSize(520, 400);
    buildUi();
}

void RecipeChangeDialog::buildUi()
{
    m_recipeList = new QListWidget(this);
    m_paramStack = new QStackedWidget(this);

    auto* emptyLabel = new QLabel(tr("No configurable parameters."), this);
    emptyLabel->setAlignment(Qt::AlignCenter);
    int emptyIdx = m_paramStack->addWidget(emptyLabel);

    for (const auto& desc : m_descriptors) {
        m_recipeList->addItem(desc.name);

        if (desc.createWidget) {
            auto* w = desc.createWidget(m_paramStack);
            m_stackIndices.append(m_paramStack->addWidget(w));
        } else {
            m_stackIndices.append(emptyIdx);
        }
    }

    auto* topLayout = new QHBoxLayout;
    topLayout->addWidget(m_recipeList, 1);
    topLayout->addWidget(m_paramStack, 2);

    auto* dimGroup = new QGroupBox(tr("Field dimensions"), this);
    auto* dimForm  = new QFormLayout(dimGroup);

    m_widthSpin = new QSpinBox(this);
    m_widthSpin->setRange(1, 100000);
    m_widthSpin->setValue(static_cast<int>(m_currentWidth));

    m_heightSpin = new QSpinBox(this);
    m_heightSpin->setRange(1, 100000);
    m_heightSpin->setValue(static_cast<int>(m_currentHeight));

    m_planesSpin = new QSpinBox(this);
    m_planesSpin->setRange(1, 64);
    m_planesSpin->setValue(static_cast<int>(m_currentPlanes));

    dimForm->addRow(tr("Width:"),  m_widthSpin);
    dimForm->addRow(tr("Height:"), m_heightSpin);
    dimForm->addRow(tr("Planes:"), m_planesSpin);

    m_warningLabel = new QLabel(this);
    m_warningLabel->setStyleSheet(
        QStringLiteral("QLabel { color: #cc6600; font-weight: bold; }"));
    m_warningLabel->setWordWrap(true);
    m_warningLabel->hide();

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout, 1);
    mainLayout->addWidget(dimGroup);
    mainLayout->addWidget(m_warningLabel);
    mainLayout->addWidget(buttons);

    connect(m_recipeList, &QListWidget::currentRowChanged,
            this, &RecipeChangeDialog::onRecipeSelected);
    connect(m_widthSpin, &QSpinBox::valueChanged,
            this, &RecipeChangeDialog::onDimensionsChanged);
    connect(m_heightSpin, &QSpinBox::valueChanged,
            this, &RecipeChangeDialog::onDimensionsChanged);
    connect(m_planesSpin, &QSpinBox::valueChanged,
            this, &RecipeChangeDialog::onDimensionsChanged);
    connect(buttons, &QDialogButtonBox::accepted,
            this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    if (!m_descriptors.isEmpty())
        m_recipeList->setCurrentRow(0);
}

void RecipeChangeDialog::onRecipeSelected(int row)
{
    if (row < 0 || row >= m_descriptors.size())
        return;

    m_paramStack->setCurrentIndex(m_stackIndices[row]);
    m_planesSpin->setValue(
        static_cast<int>(m_descriptors[row].defaultNumPlanes));
    updateWarning();
}

void RecipeChangeDialog::onDimensionsChanged()
{
    updateWarning();
}

void RecipeChangeDialog::updateWarning()
{
    size_t newW = static_cast<size_t>(m_widthSpin->value());
    size_t newH = static_cast<size_t>(m_heightSpin->value());
    size_t newP = static_cast<size_t>(m_planesSpin->value());

    bool needsReset = (newW != m_currentWidth)
                   || (newH != m_currentHeight)
                   || (newP < m_currentPlanes);

    if (needsReset) {
        QString reason;
        if (newW != m_currentWidth || newH != m_currentHeight)
            reason = tr("Field dimensions differ from current.");
        else
            reason = tr("Fewer planes than current — data will be lost.");

        m_warningLabel->setText(
            tr("⚠ Warning: %1 Planes will be reset to zero.").arg(reason));
        m_warningLabel->show();
    } else {
        m_warningLabel->hide();
    }
}

AutomatonFactory RecipeChangeDialog::buildFactory(int recipeIndex) const
{
    const auto& desc = m_descriptors[recipeIndex];

    auto* paramWidget = qobject_cast<RecipeParameterWidget*>(
        m_paramStack->widget(m_stackIndices[recipeIndex]));

    if (paramWidget) {
        return [paramWidget](Planes& planes) {
            return paramWidget->createAutomaton(planes);
        };
    }

    return desc.factory;
}

RecipeChangeResult RecipeChangeDialog::result() const
{
    int row = m_recipeList->currentRow();

    RecipeChangeResult r;
    r.width     = static_cast<size_t>(m_widthSpin->value());
    r.height    = static_cast<size_t>(m_heightSpin->value());
    r.numPlanes = static_cast<size_t>(m_planesSpin->value());
    r.factory   = const_cast<RecipeChangeDialog*>(this)->buildFactory(row);
    return r;
}

} // namespace ntcap