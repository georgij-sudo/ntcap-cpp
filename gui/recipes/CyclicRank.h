#pragma once

#include "Automaton.h"
#include "TorusTopology.h"
#include <random>

namespace ntcap {

/**
 * @brief CyclicRank — cyclic automaton with probabilistic Von Neumann rule.
 *
 * Single plane. Cell states in [0, statesCount).
 * On each step, with probability (probability/100), a cell transitions
 * to the next cyclic state if at least one Von Neumann neighbour
 * already has that state.
 *
 * @param statesCount  number of cyclic states (>= 2).
 * @param probability  percent chance per cell per step, [0..100].
 */
class CyclicRank final : public Automaton {
    Q_OBJECT
public:
    explicit CyclicRank(Planes& current,
                        int statesCount = 4,
                        int probability = 25,
                        QObject* parent = nullptr);
    ~CyclicRank() override;

    void step() override;
    QString name() const override;
    QString description() const override;

    int statesCount() const noexcept;
    int probability() const noexcept;

private:
    TorusTopology m_topo;
    int m_statesCount;
    int m_probability;
    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_dist;
};

} // namespace ntcap