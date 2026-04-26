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

} // namespace ntcap