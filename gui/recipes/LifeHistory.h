#pragma once

#include "Automaton.h"

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
                         std::unique_ptr<Topology> topology,
                         QObject* parent = nullptr);
    ~LifeHistory() override;

    void step() override;

    /**
     * Read-only view of the previous generation (plane 1).
     */
    std::span<const uint8_t> previous() const noexcept;
};

} // namespace ntcap