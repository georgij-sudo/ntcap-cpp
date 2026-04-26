#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

/**
 * @brief Histogram — gravity-like particle automaton on 2 planes.
 *
 * Plane 0: particles (values > 0 fall southward).
 * Plane 1: static walls (top and bottom rows, immutable).
 *
 * Uses Von Neumann neighbourhood. Topology: torus.
 * Requires 2 planes.
 */
class Histogram final : public Automaton {
    Q_OBJECT
public:
    explicit Histogram(Planes& current, QObject* parent = nullptr);
    ~Histogram() override;

    void step() override;
    void setCurrent(Planes& current) override;
    QString name() const override;
    QString description() const override;

private:
    TorusTopology m_topo;
};

} // namespace ntcap