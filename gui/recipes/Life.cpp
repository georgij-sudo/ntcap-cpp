#include "Life.h"

namespace ntcap {

Life::Life(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
}

Life::~Life() = default;

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
                (cur[rowN + colW] != 0) +   // NW
                (cur[rowN + x   ] != 0) +   // N
                (cur[rowN + colE] != 0) +   // NE
                (cur[rowC + colW] != 0) +   // W
                (cur[rowC + colE] != 0) +   // E
                (cur[rowS + colW] != 0) +   // SW
                (cur[rowS + x   ] != 0) +   // S
                (cur[rowS + colE] != 0);    // SE

            const size_t idx = rowC + x;

            // B3/S23
            if (cur[idx] != 0)
                next[idx] = (alive == 2 || alive == 3) ? 1 : 0;
            else
                next[idx] = (alive == 3) ? 1 : 0;
        }
    }
}

} // namespace ntcap