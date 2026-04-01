#include "Life.h"
#include "Coord2D.h"
#include "UCoord2D.h"
#include "MooreN.h"

namespace ntcap {

Life::Life(Planes& current,
           std::unique_ptr<Topology> topology,
           QObject* parent)
    : Automaton(current, std::move(topology), parent)
{
}

Life::~Life() = default;

void Life::step()
{
    const size_t  W    = current().width();
    const size_t  H    = current().height();
    const uint8_t* cur  = current().data();
    uint8_t*       next = tmp().data();

    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x) {
            const auto px = static_cast<ptrdiff_t>(x);
            const auto py = static_cast<ptrdiff_t>(y);

            const UCoord2D nc  = topology().normalize({px,     py - 1});
            const UCoord2D nec = topology().normalize({px + 1, py - 1});
            const UCoord2D ec  = topology().normalize({px + 1, py    });
            const UCoord2D sec = topology().normalize({px + 1, py + 1});
            const UCoord2D sc  = topology().normalize({px,     py + 1});
            const UCoord2D swc = topology().normalize({px - 1, py + 1});
            const UCoord2D wc  = topology().normalize({px - 1, py    });
            const UCoord2D nwc = topology().normalize({px - 1, py - 1});

            const size_t idx = y * W + x;

            const MooreN hood = MooreNBuilder()
                .center   (cur[idx])
                .north    (cur[nc.y  * W + nc.x])
                .northEast(cur[nec.y * W + nec.x])
                .east     (cur[ec.y  * W + ec.x])
                .southEast(cur[sec.y * W + sec.x])
                .south    (cur[sc.y  * W + sc.x])
                .southWest(cur[swc.y * W + swc.x])
                .west     (cur[wc.y  * W + wc.x])
                .northWest(cur[nwc.y * W + nwc.x])
                .build();

            // count живых соседей (без центра)
            const uint8_t alive = hood.countIf([](uint8_t v) { return v != 0; })
                                - (hood.center() != 0 ? 1 : 0);

            // B3/S23: рождение при 3, выживание при 2 или 3
            if (hood.center() != 0) {
                next[idx] = (alive == 2 || alive == 3) ? 1 : 0;
            } else {
                next[idx] = (alive == 3) ? 1 : 0;
            }
        }
    }
}

} // namespace ntcap