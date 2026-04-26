#include "TrailPostProcessor.h"

#include <QRgb>
#include <stdexcept>
#include <algorithm>
#include <cmath>

namespace ntcap {

TrailPostProcessor::TrailPostProcessor(double decay)
    : m_decay(decay)
{
    if (decay <= 0.0 || decay >= 1.0)
        throw std::invalid_argument("TrailPostProcessor: decay must be in (0, 1)");
}

void TrailPostProcessor::process(QImage& image)
{
    if (image.format() != QImage::Format_ARGB32)
        throw std::invalid_argument("TrailPostProcessor: expected ARGB32");

    if (m_trail.isNull() || m_trail.size() != image.size()) {
        m_trail = image.copy();
        return;
    }

    const int w = image.width();
    const int h = image.height();
    const double d = m_decay;

    for (int y = 0; y < h; ++y) {
        auto* cur = reinterpret_cast<QRgb*>(image.scanLine(y));
        auto* trail = reinterpret_cast<QRgb*>(m_trail.scanLine(y));

        for (int x = 0; x < w; ++x) {
            const int tr = std::max(qRed(cur[x]),   static_cast<int>(qRed(trail[x])   * d + 0.5));
            const int tg = std::max(qGreen(cur[x]), static_cast<int>(qGreen(trail[x]) * d + 0.5));
            const int tb = std::max(qBlue(cur[x]),  static_cast<int>(qBlue(trail[x])  * d + 0.5));

            const QRgb blended = qRgba(std::clamp(tr, 0, 255),
                                       std::clamp(tg, 0, 255),
                                       std::clamp(tb, 0, 255),
                                       qAlpha(cur[x]));
            cur[x]   = blended;
            trail[x] = blended;
        }
    }

    m_trail = image.copy();
}

void TrailPostProcessor::reset()
{
    m_trail = QImage{};
}

QString TrailPostProcessor::name() const
{
    return QStringLiteral("Trail");
}

void TrailPostProcessor::setDecay(double decay)
{
    if (decay <= 0.0 || decay >= 1.0)
        throw std::invalid_argument("TrailPostProcessor: decay must be in (0, 1)");
    m_decay = decay;
}

double TrailPostProcessor::decay() const noexcept
{
    return m_decay;
}

} // namespace ntcap