#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

/**
 * @brief TimeTunnel — reversible two-plane automaton (Von Neumann).
 *
 * Plane 0: current generation.
 * Plane 1: previous generation.
 *
 * Reversibility: swapping plane 0 ↔ plane 1 via Planes::swapPlanes()
 * reverses the direction of time.
 *
 * Requires Planes(w, h, 2).
 */
class TimeTunnel final : public Automaton {
    Q_OBJECT
public:
    explicit TimeTunnel(Planes& current, QObject* parent = nullptr);
    ~TimeTunnel() override;

    void step() override;
    QString name() const override;
    QString description() const override;

private:
    TorusTopology m_topo;
};

} // namespace ntcap