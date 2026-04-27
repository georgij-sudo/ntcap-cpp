#include "StableTubeWorms.h"

namespace ntcap {

static constexpr std::array<bool,9> STABLE_ALARM = {
    false, false, true, false,
    true,  true,  true, true, true
};

StableTubeWorms::StableTubeWorms(Planes& current, QObject* parent)
    : TubeWorms(current, STABLE_ALARM, parent)
{
}

QString StableTubeWorms::name() const
{
    return QStringLiteral("StableTubeWorms");
}

QString StableTubeWorms::description() const
{
    return QStringLiteral(
        "<h2>StableTubeWorms</h2>"
        "<p>A variant of <i>TubeWorms</i> with a predefined alarm mask "
        "that produces stable, self-sustaining tube structures.</p>"
        "<h3>Alarm mask</h3>"
        "<table border='1' cellpadding='4' cellspacing='0'>"
        "<tr><th>Neighbours</th>"
        "<th>0</th><th>1</th><th>2</th><th>3</th>"
        "<th>4</th><th>5</th><th>6</th><th>7</th><th>8</th></tr>"
        "<tr><td><b>Alarm</b></td>"
        "<td>✗</td><td>✗</td><td>✓</td><td>✗</td>"
        "<td>✓</td><td>✓</td><td>✓</td><td>✓</td><td>✓</td></tr>"
        "</table>"
        "<h3>Behaviour</h3>"
        "<p>The alarm fires when 2 or ≥ 4 Moore neighbours are alive. "
        "This creates a balance between growth and suppression, "
        "resulting in worms that extend, branch, and eventually stabilise "
        "into static tubular networks.</p>"
        "<h3>Planes</h3>"
        "<table border='1' cellpadding='4' cellspacing='0'>"
        "<tr><th>Plane</th><th>Meaning</th><th>Values</th></tr>"
        "<tr><td>0</td><td>Worm body</td><td>0 or 1</td></tr>"
        "<tr><td>1</td><td>Alarm signal</td><td>0 or 1</td></tr>"
        "<tr><td>2</td><td>Countdown timer</td><td>0, 1, 2, 3</td></tr>"
        "</table>"
        "<h3>Topology</h3>"
        "<p>Torus (wrapping edges).</p>"
        "<p>Requires <b>3</b> planes.</p>"
    );
}

} // namespace ntcap