#include "BrianBrain.h"
#include <stdexcept>

namespace ntcap {

BrianBrain::BrianBrain(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
    if (current.numPlanes() < 2)
        throw std::invalid_argument("BrianBrain requires at least 2 planes");
}

BrianBrain::~BrianBrain() = default;

void BrianBrain::setCurrent(Planes& current)
{
    Automaton::setCurrent(current);
    m_topo = TorusTopology(current.width(), current.height());
}

QString BrianBrain::name() const
{
    return QStringLiteral("BrianBrain");
}

QString BrianBrain::description() const
{
    return QStringLiteral(
        "<h2>Brian's Brain</h2>"
        "<p>A three-state cellular automaton on Moore neighbourhood (8 cells), "
        "encoded across two planes.</p>"
        "<h3>State encoding</h3>"
        "<table border='1' cellpadding='4' cellspacing='0'>"
        "<tr><th>plane0</th><th>plane1</th><th>State</th><th>Display value</th></tr>"
        "<tr><td>0</td><td>0</td><td>Ready (dead)</td><td>0</td></tr>"
        "<tr><td>1</td><td>0</td><td>Firing</td><td>1</td></tr>"
        "<tr><td>0</td><td>1</td><td>Dying (refractory)</td><td>2</td></tr>"
        "</table>"
        "<h3>Rule</h3>"
        "<ul>"
        "<li><b>Ready → Firing:</b> a ready cell (state 0) fires "
        "iff exactly 2 Moore neighbours in plane0 are firing.</li>"
        "<li><b>Firing → Dying:</b> previous plane0 is copied to plane1.</li>"
        "<li><b>Dying → Ready:</b> after one step as dying, both planes become 0.</li>"
        "</ul>"
        "<h3>Display</h3>"
        "<p>Use <i>Bitfield</i> display mode: value = plane0 + 2·plane1.</p>"
        "<h3>Planes</h3>"
        "<p>Requires <b>2</b> planes.</p>"
        "<h3>Topology</h3>"
        "<p>Torus (wrapping edges).</p>"
    );
}

void BrianBrain::step()
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
            const auto px = static_cast<ptrdiff_t>(x);
            const size_t colW = m_topo.wrapX(px - 1);
            const size_t colE = m_topo.wrapX(px + 1);

            const size_t idx = rowC + x;

            bool ready = (cur0[idx] + 2 * cur1[idx]) == 0;

            const uint8_t firingNeighbours =
                (cur0[rowN + colW] != 0) +
                (cur0[rowN + x   ] != 0) +
                (cur0[rowN + colE] != 0) +
                (cur0[rowC + colW] != 0) +
                (cur0[rowC + colE] != 0) +
                (cur0[rowS + colW] != 0) +
                (cur0[rowS + x   ] != 0) +
                (cur0[rowS + colE] != 0);

            bool stimulus = (firingNeighbours == 2);

            next0[idx] = (ready && stimulus) ? 1 : 0;
            next1[idx] = cur0[idx];
        }
    }
}

} // namespace ntcap