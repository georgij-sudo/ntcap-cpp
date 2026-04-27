#pragma once

#include "Automaton.h"
#include "TorusTopology.h"
#include <random>

namespace ntcap {

class CyclicRank final : public Automaton {
    Q_OBJECT
public:
    explicit CyclicRank(Planes& current,
                        int statesCount = 4,
                        int probability = 25,
                        QObject* parent = nullptr);
    ~CyclicRank() override;

    void step() override;
    void setCurrent(Planes& current) override;
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