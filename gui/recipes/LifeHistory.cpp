#include "LifeHistory.h"

#include <cstring>

namespace ntcap {

LifeHistory::LifeHistory(Planes& current, QObject* parent)
    : Automaton(current, parent)          // topology не используется через базу
    , m_topo(current.width(), current.height())
{
    Q_ASSERT(current.numPlanes() >= 2);
}

LifeHistory::~LifeHistory() = default;

void LifeHistory::step()
{
    const size_t W         = current().width();
    const size_t H         = current().height();
    const size_t planeSize = current().size();          // W * H

    const uint8_t* cur  = current().data();             // plane 0 — current gen
    uint8_t*       next = tmp().data();                 // plane 0 of tmp — next gen
    uint8_t*       prev = tmp().data() + planeSize;     // plane 1 of tmp — snapshot

    // ── save current generation into plane 1 ──
    std::memcpy(prev, cur, planeSize);

    // ── compute next generation ──
    for (size_t y = 0; y < H; ++y) {

        const auto py = static_cast<ptrdiff_t>(y);

        // строковые смещения — вычисляются один раз на строку
        const size_t rowN = m_topo.wrapY(py - 1) * W;
        const size_t rowC = y * W;
        const size_t rowS = m_topo.wrapY(py + 1) * W;

        for (size_t x = 0; x < W; ++x) {

            const auto px = static_cast<ptrdiff_t>(x);

            const size_t colW = m_topo.wrapX(px - 1);
            const size_t colE = m_topo.wrapX(px + 1);

            // 8 соседей — прямой доступ, ни одного виртуального вызова
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

std::span<const uint8_t> LifeHistory::previous() const noexcept
{
    return current().view(1);
}

} // namespace ntcap