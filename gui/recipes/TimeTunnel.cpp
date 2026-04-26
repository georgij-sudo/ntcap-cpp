#include "TimeTunnel.h"
#include "VonNeumannN.h"
#include <stdexcept>

namespace ntcap {

TimeTunnel::TimeTunnel(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
    if (current.numPlanes() < 2)
        throw std::invalid_argument("TimeTunnel requires at least 2 planes");
}

TimeTunnel::~TimeTunnel() = default;

void TimeTunnel::step()
{
    const auto w = current().width();
    const auto h = current().height();
    const auto planeSize = current().size();

    auto plane0 = current().view(0);
    auto plane1 = current().view(1);

    uint8_t* dst0 = tmp().data();
    uint8_t* dst1 = tmp().data() + planeSize;

    for (size_t y = 0; y < h; ++y) {
        for (size_t x = 0; x < w; ++x) {
            size_t idx = y * w + x;

            auto vn = VonNeumannNBuilder()
                .center(plane0[idx])
                .north(plane0[m_topo.wrapY(static_cast<ptrdiff_t>(y) - 1) * w + x])
                .south(plane0[m_topo.wrapY(static_cast<ptrdiff_t>(y) + 1) * w + x])
                .west (plane0[y * w + m_topo.wrapX(static_cast<ptrdiff_t>(x) - 1)])
                .east (plane0[y * w + m_topo.wrapX(static_cast<ptrdiff_t>(x) + 1)])
                .build();

            uint8_t total = vn.count() + vn.center();
            uint8_t val = (total != 0 && total != 5) ? 1u : 0u;
            val ^= plane1[idx];

            dst0[idx] = val;
            dst1[idx] = plane0[idx];
        }
    }
    commit();
}

QString TimeTunnel::name() const
{
    return QStringLiteral("TimeTunnel");
}

QString TimeTunnel::description() const
{
    return QStringLiteral(
        "<h2>TimeTunnel</h2>"
        "<p>Two-plane reversible cellular automaton with Von Neumann neighbourhood.</p>"
        "<h3>Rule</h3>"
        "<p>"
        "count = Σ VonNeumann(plane₀) + center<br>"
        "new₀ = (count ≠ 0 ∧ count ≠ 5) ? 1 : 0<br>"
        "new₀ = new₀ ⊕ plane₁[i]<br>"
        "new₁ = plane₀[i]"
        "</p>"
        "<h3>Reversibility</h3>"
        "<p>Swapping plane₀ ↔ plane₁ (Planes → Swap Planes) "
        "reverses the direction of time.</p>"
        "<h3>Planes</h3>"
        "<p>Requires <b>2</b> planes.</p>"
    );
}

} // namespace ntcap