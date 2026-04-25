#pragma once

#include <QObject>
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

#include "Automaton.h"
#include "Planes.h"
#include "TorusTopology.h"

namespace ntcap {

/**
 * @brief TubeWorms cellular automaton (non-owning Planes).
 *
 * Plane 0: worm/body (0 or 1)
 * Plane 1: alarm (0 or 1)
 * Plane 2: countdown/timer (0..3)
 *
 * The constructor signature matches LifeHistory: TubeWorms(Planes& current, QObject* parent = nullptr).
 */
class TubeWorms : public Automaton {
    Q_OBJECT
public:
    /**
     * @brief Construct TubeWorms bound to externally managed Planes.
     *
     * @param current Planes reference (must have numPlanes() >= 3).
     * @param parent optional QObject parent.
     *
     * @throws std::invalid_argument if current.numPlanes() < 3.
     */
    explicit TubeWorms(Planes& current, QObject* parent = nullptr);

    ~TubeWorms() override;

    /**
     * @brief Performs one step of the automaton into tmp() (does not commit()).
     *
     * @throws std::out_of_range if cell values violate expected ranges.
     */
    void step() override;

protected:
    /**
     * @brief Protected constructor allowing derived classes to provide custom alarm mask.
     *
     * @param current Planes reference (must have numPlanes() >= 3).
     * @param alarmArray alarm mask (size 9).
     * @param parent optional QObject parent.
     *
     * @throws std::invalid_argument if current.numPlanes() < 3.
     */
    TubeWorms(Planes& current,
              const std::array<bool,9>& alarmArray,
              QObject* parent = nullptr);

private:
    TorusTopology m_topo;
    std::array<bool,9> m_alarm;

    static constexpr std::array<uint8_t,4> WORM = {1u, 0u, 0u, 0u};
    static constexpr std::array<uint8_t,4> TIMER = {0u, 0u, 1u, 2u};
};

} // namespace ntcap