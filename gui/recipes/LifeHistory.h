#pragma once

#include "Automaton.h"
#include "TorusTopology.h"

namespace ntcap {

/**
 * Conway's Game of Life with history.
 *
 * Plane 0 — current generation.
 * Plane 1 — previous generation (read-only snapshot).
 *
 * The caller must supply Planes(width, height, 2).
 */
class LifeHistory final : public Automaton {
    Q_OBJECT
public:
    explicit LifeHistory(Planes& current,
                         QObject* parent = nullptr);
    ~LifeHistory() override;

    void step() override;
    QString name() const override;

    /**
     * Read-only view of the previous generation (plane 1).
     */
    std::span<const uint8_t> previous() const noexcept;

private:
    TorusTopology m_topo;
};

} // namespace ntcap