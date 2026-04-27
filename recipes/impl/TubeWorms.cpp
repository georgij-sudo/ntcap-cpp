#include "TubeWorms.h"

#include <cstring>

namespace ntcap {

TubeWorms::TubeWorms(Planes& current, QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
    , m_alarm{}
{
    if (current.numPlanes() < 3)
        throw std::invalid_argument{"TubeWorms requires Planes with at least 3 planes"};
}

TubeWorms::TubeWorms(Planes& current,
                     const std::array<bool,9>& alarmArray,
                     QObject* parent)
    : Automaton(current, parent)
    , m_topo(current.width(), current.height())
    , m_alarm(alarmArray)
{
    if (current.numPlanes() < 3)
        throw std::invalid_argument{"TubeWorms requires Planes with at least 3 planes"};
}

TubeWorms::~TubeWorms() = default;

void TubeWorms::setCurrent(Planes& current){
    Automaton::setCurrent(current);
    m_topo = TorusTopology(current.width(), current.height());
}

QString TubeWorms::name() const
{
    return QStringLiteral("TubeWorms");
}

QString TubeWorms::description() const
{
    return QStringLiteral(
        "<h2>TubeWorms</h2>"
        "<p>A three-plane cellular automaton that produces organic, tube-like structures.</p>"
        "<h3>Planes</h3>"
        "<table border='1' cellpadding='4' cellspacing='0'>"
        "<tr><th>Plane</th><th>Meaning</th><th>Values</th></tr>"
        "<tr><td>0</td><td>Worm body</td><td>0 or 1</td></tr>"
        "<tr><td>1</td><td>Alarm signal</td><td>0 or 1</td></tr>"
        "<tr><td>2</td><td>Countdown timer</td><td>0, 1, 2, 3</td></tr>"
        "</table>"
        "<h3>Rule</h3>"
        "<p>"
        "Let <i>n</i> = count of Moore neighbours with body = 1.<br>"
        "Let <i>t</i> = current countdown value."
        "</p>"
        "<p>"
        "WORM lookup: [1, 0, 0, 0] — body is alive only when timer = 0.<br>"
        "TIMER lookup: [0, 0, 1, 2] — timer counts down each step."
        "</p>"
        "<p>"
        "new_body = WORM[t]<br>"
        "new_alarm = alarm_mask[n]<br>"
        "new_timer = (body = 1 ∧ alarm = 1) ? 3 : TIMER[t]"
        "</p>"
        "<h3>Alarm mask</h3>"
        "<p>The alarm mask is an array of 9 booleans indexed by neighbour count. "
        "Different masks produce different worm morphologies. "
        "The base TubeWorms uses an all-false mask (no alarm). "
        "See <i>StableTubeWorms</i> for a predefined variant.</p>"
        "<h3>Topology</h3>"
        "<p>Torus (wrapping edges).</p>"
        "<p>Requires <b>3</b> planes.</p>"
    );
}

void TubeWorms::step()
{
    const size_t W = current().width();
    const size_t H = current().height();
    const size_t planeSize = current().size();

    const uint8_t* cur = current().data();
    uint8_t* next = tmp().data();

    const uint8_t* cur0 = cur;
    const uint8_t* cur1 = cur + planeSize;
    const uint8_t* cur2 = cur + 2 * planeSize;

    uint8_t* next0 = next;
    uint8_t* next1 = next + planeSize;
    uint8_t* next2 = next + 2 * planeSize;

    for (size_t y = 0; y < H; ++y) {

        const auto py = static_cast<ptrdiff_t>(y);

        const size_t rowN = m_topo.wrapY(py - 1) * W;
        const size_t rowC = y * W;
        const size_t rowS = m_topo.wrapY(py + 1) * W;

        for (size_t x = 0; x < W; ++x) {

            const auto px = static_cast<ptrdiff_t>(x);

            const size_t colW = m_topo.wrapX(px - 1);
            const size_t colE = m_topo.wrapX(px + 1);

            const size_t idxNw = rowN + colW;
            const size_t idxN  = rowN + x;
            const size_t idxNe = rowN + colE;
            const size_t idxW  = rowC + colW;
            const size_t idxC  = rowC + x;
            const size_t idxE  = rowC + colE;
            const size_t idxSw = rowS + colW;
            const size_t idxS  = rowS + x;
            const size_t idxSe = rowS + colE;

            const uint8_t vNw = cur0[idxNw];
            const uint8_t vN  = cur0[idxN];
            const uint8_t vNe = cur0[idxNe];
            const uint8_t vW  = cur0[idxW];
            const uint8_t vC  = cur0[idxC];
            const uint8_t vE  = cur0[idxE];
            const uint8_t vSw = cur0[idxSw];
            const uint8_t vS  = cur0[idxS];
            const uint8_t vSe = cur0[idxSe];

            if (vNw > 1u || vN > 1u || vNe > 1u || vW > 1u || vC > 1u ||
                vE > 1u || vSw > 1u || vS > 1u || vSe > 1u) {
                throw std::out_of_range{"invalid value in plane 0 (expected 0 or 1)"};
            }

            const int neighboursCount =
                (vNw == 1u) + (vN == 1u) + (vNe == 1u) +
                (vW  == 1u)               + (vE  == 1u) +
                (vSw == 1u) + (vS == 1u)  + (vSe == 1u);

            const uint8_t countdown = cur2[idxC];
            if (countdown >= TIMER.size()) {
                throw std::out_of_range{"invalid countdown value in plane 2"};
            }

            next0[idxC] = WORM[static_cast<size_t>(countdown)];
            next1[idxC] = (m_alarm.at(static_cast<size_t>(neighboursCount)) ? 1u : 0u);

            const uint8_t plane1Val = cur1[idxC];
            if (vC == 1u && plane1Val == 1u) {
                next2[idxC] = static_cast<uint8_t>(TIMER.size() - 1u);
            } else {
                next2[idxC] = TIMER[static_cast<size_t>(countdown)];
            }
        }
    }
}

} // namespace ntcap