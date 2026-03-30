#include "AutomatonController.h"
#include "VonNeumannN.h"
#include "Coord2D.h"
#include "UCoord2D.h"

#include <cstring>

namespace ntcap {

// ─── Конструктор ────────────────────────────────────────────────────

AutomatonController::AutomatonController(Planes planes,
                                         TorusTopology topology,
                                         TableRule<6> rule,
                                         QObject* parent)
    : QObject(parent)
    , m_planes(std::move(planes))
    , m_topology(std::move(topology))
    , m_rule(std::move(rule))
{
    connect(&m_timer, &QTimer::timeout,
            this,     &AutomatonController::step);
}

// ─── Геттеры ────────────────────────────────────────────────────────

const Planes* AutomatonController::planes() const noexcept
{
    return &m_planes;
}

size_t AutomatonController::generation() const noexcept
{
    return m_generation;
}

// ─── Один шаг симуляции ─────────────────────────────────────────────

void AutomatonController::step()
{
    const size_t W = m_planes.width();
    const size_t H = m_planes.height();

    const std::span<const uint8_t> cur = m_planes.view(0);
    uint8_t* next = m_planes.data() + m_planes.size();

    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x) {

            const auto px = static_cast<ptrdiff_t>(x);
            const auto py = static_cast<ptrdiff_t>(y);

            const UCoord2D nc = m_topology.normalize({px,     py - 1});
            const UCoord2D ec = m_topology.normalize({px + 1, py    });
            const UCoord2D sc = m_topology.normalize({px,     py + 1});
            const UCoord2D wc = m_topology.normalize({px - 1, py    });

            const size_t idx = y * W + x;

            const VonNeumannN neighborhood = VonNeumannNBuilder()
                .center(cur[idx])
                .north (cur[nc.y * W + nc.x])
                .east  (cur[ec.y * W + ec.x])
                .south (cur[sc.y * W + sc.x])
                .west  (cur[wc.y * W + wc.x])
                .build();

            next[idx] = m_rule(neighborhood.count());
        }
    }

    std::memcpy(m_planes.data(), next, m_planes.size());

    ++m_generation;
    emit stepped();
}

// ─── Управление таймером ────────────────────────────────────────────

void AutomatonController::start()
{
    m_timer.start();
}

void AutomatonController::stop()
{
    m_timer.stop();
}

void AutomatonController::setInterval(int ms)
{
    m_timer.setInterval(ms);
}

} // namespace ntcap