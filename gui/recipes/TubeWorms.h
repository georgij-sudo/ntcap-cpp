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
 * The constructor signature matches LifeHistory.
 */
class TubeWorms : public Automaton {
    Q_OBJECT
public:
    explicit TubeWorms(Planes& current, QObject* parent = nullptr);
    ~TubeWorms() override;

    void step() override;
    QString name() const override;

protected:
    TubeWorms(Planes& current,
              const std::array<bool,9>& alarmArray,
              QObject* parent = nullptr);

private:
    TorusTopology m_topo;
    std::array<bool,9> m_alarm;

    static constexpr std::array<uint8_t,4> WORM  = {1u, 0u, 0u, 0u};
    static constexpr std::array<uint8_t,4> TIMER = {0u, 0u, 1u, 2u};
};

} // namespace ntcap