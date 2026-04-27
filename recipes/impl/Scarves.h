#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

/**
 * @brief Scarves — XOR-based weaving pattern on 2 planes.
 *
 * The first row evolves by XOR-sum rules (Von Neumann neighbours).
 * All subsequent rows shift the pattern downward (copy from north).
 *
 * Requires 2 planes. Topology: torus.
 */
class Scarves final : public Automaton {
    Q_OBJECT
public:
    explicit Scarves(Planes& current, QObject* parent = nullptr);
    ~Scarves() override;

    void step() override;
    void setCurrent(Planes& current) override;
    QString name() const override;
    QString description() const override;

private:
    TorusTopology m_topo;
};

} // namespace ntcap