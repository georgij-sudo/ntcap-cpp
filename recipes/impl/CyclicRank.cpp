#include "CyclicRank.h"
#include "VonNeumannN.h"
#include <stdexcept>
#include <chrono>

namespace ntcap {

CyclicRank::CyclicRank(Planes& current,
                       int statesCount,
                       int probability,
                       QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
    , m_statesCount(statesCount)
    , m_probability(probability)
    , m_rng(static_cast<unsigned>(
          std::chrono::steady_clock::now().time_since_epoch().count()))
    , m_dist(0, 100)
{
    if (statesCount < 2)
        throw std::invalid_argument("CyclicRank: statesCount must be >= 2");
    if (probability < 0 || probability > 100)
        throw std::invalid_argument("CyclicRank: probability must be in [0, 100]");
}

CyclicRank::~CyclicRank() = default;

void CyclicRank::step()
{
    const auto w = current().width();
    const auto h = current().height();
    auto plane = current().view(0);
    uint8_t* dst = tmp().data();

    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            size_t idx = y * w + x;

            if (m_dist(m_rng) > m_probability) {
                dst[idx] = plane[idx];
                continue;
            }

            uint8_t stronger = static_cast<uint8_t>(
                (plane[idx] + 1) % m_statesCount);

            uint8_t n = plane[m_topo.wrapY(static_cast<ptrdiff_t>(y) - 1) * w + x];
            uint8_t s = plane[m_topo.wrapY(static_cast<ptrdiff_t>(y) + 1) * w + x];
            uint8_t we = plane[y * w + m_topo.wrapX(static_cast<ptrdiff_t>(x) - 1)];
            uint8_t e = plane[y * w + m_topo.wrapX(static_cast<ptrdiff_t>(x) + 1)];

            bool hasStronger = (n == stronger)
                            || (s == stronger)
                            || (we == stronger)
                            || (e == stronger);

            dst[idx] = hasStronger ? stronger : plane[idx];
        }
    }
    commit();
}

void CyclicRank::setCurrent(Planes& current)
{
    Automaton::setCurrent(current);
    m_topo = TorusTopology(current.width(), current.height());
}

QString CyclicRank::name() const
{
    return QStringLiteral("CyclicRank(%1, %2%)")
        .arg(m_statesCount)
        .arg(m_probability);
}

QString CyclicRank::description() const
{
    return QStringLiteral(
        "<h2>CyclicRank</h2>"
        "<p>Cyclic cellular automaton with probabilistic Von Neumann rule.</p>"
        "<h3>Parameters</h3>"
        "<ul>"
        "<li><b>statesCount</b> = %1 — number of cyclic states [0, N)</li>"
        "<li><b>probability</b> = %2%% — chance to apply rule per cell per step</li>"
        "</ul>"
        "<h3>Rule</h3>"
        "<p>"
        "stronger = (cell + 1) mod N<br>"
        "if rand(0..100) ≤ probability ∧ ∃ VN-neighbour = stronger:<br>"
        "&nbsp;&nbsp;&nbsp;&nbsp;cell ← stronger<br>"
        "else:<br>"
        "&nbsp;&nbsp;&nbsp;&nbsp;cell ← cell"
        "</p>"
        "<h3>Planes</h3>"
        "<p>Requires <b>1</b> plane.</p>"
    ).arg(m_statesCount).arg(m_probability);
}

int CyclicRank::statesCount() const noexcept
{
    return m_statesCount;
}

int CyclicRank::probability() const noexcept
{
    return m_probability;
}

} // namespace ntcap