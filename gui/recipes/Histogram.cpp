#include "Histogram.h"
#include <stdexcept>

namespace ntcap {

Histogram::Histogram(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
    if (current.numPlanes() < 2)
        throw std::invalid_argument("Histogram requires at least 2 planes");
}

Histogram::~Histogram() = default;

void Histogram::setCurrent(Planes& current)
{
    Automaton::setCurrent(current);
    m_topo = TorusTopology(current.width(), current.height());
}

QString Histogram::name() const
{
    return QStringLiteral("Histogram");
}

QString Histogram::description() const
{
    return QStringLiteral(
        "<h2>Histogram</h2>"
        "<p>A two-plane automaton simulating gravity-like falling particles "
        "with static wall barriers.</p>"
        "<h3>Planes</h3>"
        "<table border='1' cellpadding='4' cellspacing='0'>"
        "<tr><th>Plane</th><th>Meaning</th></tr>"
        "<tr><td>0</td><td>Particles (values &gt; 0 fall southward)</td></tr>"
        "<tr><td>1</td><td>Static walls (immutable during step)</td></tr>"
        "</table>"
        "<h3>Initialization</h3>"
        "<p>Plane 0: random fill ~20%. "
        "Plane 1: top and bottom rows set to 1 (walls).</p>"
        "<h3>Rule (Von Neumann)</h3>"
        "<p>For each cell, using cardinal neighbours from both planes:</p>"
        "<ul>"
        "<li>If cell is empty (plane0 = 0) and north neighbour has a particle, "
        "and not blocked by wall mismatch → absorb particle from north.</li>"
        "<li>If cell has a particle (plane0 &gt; 0) and south neighbour is empty, "
        "and not blocked by wall mismatch → release particle southward.</li>"
        "</ul>"
        "<p>Plane 1 is copied unchanged.</p>"
        "<h3>Topology</h3>"
        "<p>Torus (wrapping edges).</p>"
        "<p>Requires <b>2</b> planes.</p>"
    );
}

void Histogram::step()
{
    const size_t W = current().width();
    const size_t H = current().height();
    const size_t planeSize = current().size();

    const uint8_t* cur0 = current().data();
    const uint8_t* cur1 = current().data() + planeSize;

    uint8_t* next0 = tmp().data();
    uint8_t* next1 = tmp().data() + planeSize;

    for (size_t y = 0; y < H; ++y) {
        const auto py = static_cast<ptrdiff_t>(y);
        const size_t rowN = m_topo.wrapY(py - 1) * W;
        const size_t rowC = y * W;
        const size_t rowS = m_topo.wrapY(py + 1) * W;

        for (size_t x = 0; x < W; ++x) {
            const size_t idx = rowC + x;

            uint8_t northVal0 = cur0[rowN + x];
            uint8_t northVal1 = cur1[rowN + x];
            uint8_t southVal0 = cur0[rowS + x];
            uint8_t southVal1 = cur1[rowS + x];

            uint8_t val = cur0[idx];

            if (val == 0 && northVal0 > 0
                && !(cur1[idx] > 0 && northVal1 == 0))
            {
                val = northVal0;
            }
            else if (val > 0 && southVal0 == 0
                     && !(cur1[idx] == 0 && southVal1 > 0))
            {
                val = 0;
            }

            next0[idx] = val;
            next1[idx] = cur1[idx];
        }
    }
}

} // namespace ntcap