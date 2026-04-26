#include "Scarves.h"
#include <stdexcept>

namespace ntcap {

Scarves::Scarves(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
{
    if (current.numPlanes() < 2)
        throw std::invalid_argument("Scarves requires at least 2 planes");
}

Scarves::~Scarves() = default;

void Scarves::setCurrent(Planes& current)
{
    Automaton::setCurrent(current);
    m_topo = TorusTopology(current.width(), current.height());
}

QString Scarves::name() const
{
    return QStringLiteral("Scarves");
}

QString Scarves::description() const
{
    return QStringLiteral(
        "<h2>Scarves</h2>"
        "<p>A two-plane XOR-based automaton that produces woven textile-like patterns.</p>"
        "<h3>Initialization</h3>"
        "<p>Both planes are seeded with random 0/1 values in the middle fifth "
        "of the first row. The rest of the field starts empty.</p>"
        "<h3>Rule (Von Neumann)</h3>"
        "<p><b>First row (y = 0):</b></p>"
        "<pre>"
        "sum0 = east₀ + plane1[i] + west₁ + west₀\n"
        "sum1 = east₀ + east₁ + plane0[i] + west₁\n"
        "s0 = (sum0 == 2) ? 1 : 0\n"
        "s1 = (sum1 == 2) ? 1 : 0\n"
        "new₀ = s0 ⊕ south₀\n"
        "new₁ = s1 ⊕ south₁"
        "</pre>"
        "<p><b>Other rows (y &gt; 0):</b></p>"
        "<pre>"
        "new₀ = north₀\n"
        "new₁ = north₁"
        "</pre>"
        "<p>The first row generates the pattern; subsequent rows shift it down, "
        "creating a cascading scarf-like weave.</p>"
        "<h3>Planes</h3>"
        "<p>Requires <b>2</b> planes.</p>"
        "<h3>Topology</h3>"
        "<p>Torus (wrapping edges).</p>"
    );
}

void Scarves::step()
{
    const size_t W = current().width();
    const size_t H = current().height();
    const size_t planeSize = current().size();

    const uint8_t* cur0 = current().data();
    const uint8_t* cur1 = current().data() + planeSize;

    uint8_t* next0 = tmp().data();
    uint8_t* next1 = tmp().data() + planeSize;

    for (size_t x = 0; x < W; ++x) {
        const auto px = static_cast<ptrdiff_t>(x);
        const size_t colW = m_topo.wrapX(px - 1);
        const size_t colE = m_topo.wrapX(px + 1);
        const size_t rowS = m_topo.wrapY(1) * W;

        uint8_t eastVal0 = cur0[colE];
        uint8_t westVal0 = cur0[colW];
        uint8_t eastVal1 = cur1[colE];
        uint8_t westVal1 = cur1[colW];

        int sum0 = eastVal0 + cur1[x] + westVal1 + westVal0;
        int sum1 = eastVal0 + eastVal1 + cur0[x] + westVal1;

        uint8_t s0 = (sum0 == 2) ? 1 : 0;
        uint8_t s1 = (sum1 == 2) ? 1 : 0;

        next0[x] = s0 ^ cur0[rowS + x];
        next1[x] = s1 ^ cur1[rowS + x];
    }

    for (size_t y = 1; y < H; ++y) {
        const size_t rowN = m_topo.wrapY(static_cast<ptrdiff_t>(y) - 1) * W;
        const size_t rowC = y * W;

        for (size_t x = 0; x < W; ++x) {
            next0[rowC + x] = cur0[rowN + x];
            next1[rowC + x] = cur1[rowN + x];
        }
    }
}

} // namespace ntcap