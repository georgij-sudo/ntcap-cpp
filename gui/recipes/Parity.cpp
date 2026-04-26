#include "Parity.h"

namespace ntcap {

Parity::Parity(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
}

Parity::~Parity() = default;

QString Parity::name() const
{
    return QStringLiteral("Parity");
}

QString Parity::description() const
{
    return QStringLiteral(
        "<h2>Parity</h2>"
        "<p>A totalistic rule on the Von Neumann neighbourhood (4 cells + center).</p>"
        "<h3>Rule</h3>"
        "<p>"
        "sum = center + north + east + south + west<br>"
        "cell′ = sum mod 2"
        "</p>"
        "<p>The cell is alive if the sum of itself and its four cardinal "
        "neighbours is odd, dead otherwise.</p>"
        "<h3>Behaviour</h3>"
        "<p>Produces self-similar fractal patterns (Sierpiński-like) from simple seeds. "
        "Fully deterministic and reversible in character — "
        "patterns expand symmetrically at light speed.</p>"
        "<h3>Planes</h3>"
        "<p>Requires <b>1</b> plane.</p>"
        "<h3>Topology</h3>"
        "<p>Torus (wrapping edges).</p>"
    );
}

void Parity::step()
{
    const size_t W = current().width();
    const size_t H = current().height();

    const uint8_t* cur  = current().data();
    uint8_t*       next = tmp().data();

    for (size_t y = 0; y < H; ++y) {

        const auto py = static_cast<ptrdiff_t>(y);

        const size_t rowN = m_topo.wrapY(py - 1) * W;
        const size_t rowC = y * W;
        const size_t rowS = m_topo.wrapY(py + 1) * W;

        for (size_t x = 0; x < W; ++x) {

            const auto px = static_cast<ptrdiff_t>(x);

            const size_t colW = m_topo.wrapX(px - 1);
            const size_t colE = m_topo.wrapX(px + 1);

            const unsigned sum =
                cur[rowC + x   ] +
                cur[rowN + x   ] +
                cur[rowC + colE] +
                cur[rowS + x   ] +
                cur[rowC + colW];

            next[rowC + x] = static_cast<uint8_t>(sum & 1u);
        }
    }
}

} // namespace ntcap