#pragma once

#include "RecipeParameterWidget.h"

class QSpinBox;

namespace ntcap {

/**
 * @brief Parameter editor for CyclicRank recipe.
 *
 * Controls: statesCount (2..255), probability (0..100).
 */
class CyclicRankParameterWidget final : public RecipeParameterWidget {
    Q_OBJECT
public:
    explicit CyclicRankParameterWidget(QWidget* parent = nullptr);

    std::unique_ptr<Automaton> createAutomaton(Planes& planes) override;

private:
    QSpinBox* m_statesCountSpin = nullptr;
    QSpinBox* m_probabilitySpin = nullptr;
};

} // namespace ntcap