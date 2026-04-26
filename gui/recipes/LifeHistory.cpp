#include "LifeHistory.h"

#include <cstring>

namespace ntcap {

LifeHistory::LifeHistory(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
    Q_ASSERT(current.numPlanes() >= 2);
}

LifeHistory::~LifeHistory() = default;

QString LifeHistory::name() const
{
    return QStringLiteral("LifeHistory");
}

void LifeHistory::step()
{
    const size_t W         = current().width();
    const size_t H         = current().height();
    const size_t planeSize = current().size();

    const uint8_t* cur  = current().data();
    uint8_t*       next = tmp().data();
    uint8_t*       prev = tmp().data() + planeSize;

    std::memcpy(prev, cur, planeSize);

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

std::span<const uint8_t> LifeHistory::previous() const noexcept
{
    return current().view(1);
}

} // namespace ntcap