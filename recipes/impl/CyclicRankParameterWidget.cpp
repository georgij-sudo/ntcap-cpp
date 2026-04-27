#include "CyclicRankParameterWidget.h"
#include "CyclicRank.h"

#include <QSpinBox>
#include <QFormLayout>

namespace ntcap {

CyclicRankParameterWidget::CyclicRankParameterWidget(QWidget* parent)
    : RecipeParameterWidget(parent)
{
    m_statesCountSpin = new QSpinBox(this);
    m_statesCountSpin->setRange(2, 255);
    m_statesCountSpin->setValue(4);

    m_probabilitySpin = new QSpinBox(this);
    m_probabilitySpin->setRange(0, 100);
    m_probabilitySpin->setValue(25);
    m_probabilitySpin->setSuffix(QStringLiteral("%"));

    auto* layout = new QFormLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addRow(tr("States count:"), m_statesCountSpin);
    layout->addRow(tr("Probability:"),  m_probabilitySpin);
}

std::unique_ptr<Automaton> CyclicRankParameterWidget::createAutomaton(Planes& planes)
{
    return std::make_unique<CyclicRank>(
        planes,
        m_statesCountSpin->value(),
        m_probabilitySpin->value());
}

} // namespace ntcap