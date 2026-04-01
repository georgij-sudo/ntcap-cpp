#include "Parity.h"
#include "Coord2D.h"
#include "UCoord2D.h"

namespace ntcap {

Parity::Parity(Planes& current,
               std::unique_ptr<Topology> topology,
               QObject* parent)
    : Automaton(current, std::move(topology), parent)
{
}

Parity::~Parity() = default;

void Parity::step()
{
    const size_t W = current().width();
    const size_t H = current().height();
    const uint8_t* cur  = current().data();
    uint8_t*       next = tmp().data();

    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x) {
            const auto px = static_cast<ptrdiff_t>(x);
            const auto py = static_cast<ptrdiff_t>(y);

            const UCoord2D nc = topology().normalize({px,     py - 1});
            const UCoord2D ec = topology().normalize({px + 1, py    });
            const UCoord2D sc = topology().normalize({px,     py + 1});
            const UCoord2D wc = topology().normalize({px - 1, py    });

            const size_t idx = y * W + x;

            unsigned sum = 0;
            sum += cur[idx];
            sum += cur[nc.y * W + nc.x];
            sum += cur[ec.y * W + ec.x];
            sum += cur[sc.y * W + sc.x];
            sum += cur[wc.y * W + wc.x];

            next[idx] = static_cast<uint8_t>(sum & 1u);
        }
    }
}

} // namespace ntcap