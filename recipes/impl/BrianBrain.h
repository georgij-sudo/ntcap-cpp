#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

/**
 * @brief Brian's Brain — 3-state automaton on 2 planes.
 *
 * State encoding (plane0 + 2*plane1):
 *   0 = ready (dead), 1 = firing, 2 = dying (refractory).
 *
 * Rule:
 *   - A ready cell fires iff exactly 2 Moore neighbours are firing.
 *   - plane1 stores the previous plane0 value (dying marker).
 *
 * Requires 2 planes. Topology: torus.
 */
class BrianBrain final : public Automaton {
    Q_OBJECT
public:
    explicit BrianBrain(Planes& current, QObject* parent = nullptr);
    ~BrianBrain() override;

    void step() override;
    void setCurrent(Planes& current) override;
    QString name() const override;
    QString description() const override;

private:
    TorusTopology m_topo;
};

} // namespace ntcap