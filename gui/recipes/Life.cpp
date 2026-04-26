#include "Life.h"

namespace ntcap {

Life::Life(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
}

Life::~Life() = default;

QString Life::name() const
{
    return QStringLiteral("Life");
}

QString Life::description() const
{
    return QStringLiteral(
        "<h2>Conway's Game of Life</h2>"
        "<p>The classic totalistic cellular automaton on a Moore neighbourhood (8 cells).</p>"
        "<h3>Rule — B3/S23</h3>"
        "<ul>"
        "<li><b>Birth:</b> a dead cell with exactly 3 alive neighbours becomes alive.</li>"
        "<li><b>Survival:</b> an alive cell with 2 or 3 alive neighbours stays alive.</li>"
        "<li><b>Death:</b> otherwise the cell dies (underpopulation or overpopulation).</li>"
        "</ul>"
        "<h3>Formal notation</h3>"
        "<p>"
        "Let <i>n</i> = number of alive Moore neighbours.<br>"
        "cell′ = (cell = 1 ∧ n ∈ {2, 3}) ∨ (cell = 0 ∧ n = 3)"
        "</p>"
        "<h3>Planes</h3>"
        "<p>Requires <b>1</b> plane.</p>"
        "<h3>Topology</h3>"
        "<p>Torus (wrapping edges).</p>"
    );
}

void Life::step()
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

            const uint8_t alive =
                (cur[rowN + colW] != 0) +
                (cur[rowN + x   ] != 0) +
                (cur[rowN + colE] != 0) +
                (cur[rowC + colW] != 0) +
                (cur[rowC + colE] != 0) +
                (cur[rowS + colW] != 0) +
                (cur[rowS + x   ] != 0) +
                (cur[rowS + colE] != 0);

            const size_t idx = rowC + x;

            if (cur[idx] != 0)
                next[idx] = (alive == 2 || alive == 3) ? 1 : 0;
            else
                next[idx] = (alive == 3) ? 1 : 0;
        }
    }
}

} // namespace ntcap