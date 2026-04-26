#include "AntiflickerPostProcessor.h"

#include <QRgb>
#include <stdexcept>
#include <algorithm>

namespace ntcap {

AntiflickerPostProcessor::AntiflickerPostProcessor(double alpha)
    : m_alpha(alpha)
{
    if (alpha <= 0.0 || alpha > 1.0)
        throw std::invalid_argument("AntiflickerPostProcessor: alpha must be in (0, 1]");
}

void AntiflickerPostProcessor::process(QImage& image)
{
    if (image.format() != QImage::Format_ARGB32)
        throw std::invalid_argument("AntiflickerPostProcessor: expected ARGB32");

    if (m_prev.isNull() || m_prev.size() != image.size()) {
        m_prev = image.copy();
        return;
    }

    const int w = image.width();
    const int h = image.height();
    const double a = m_alpha;
    const double b = 1.0 - a;

    for (int y = 0; y < h; ++y) {
        auto* cur  = reinterpret_cast<QRgb*>(image.scanLine(y));
        const auto* prev = reinterpret_cast<const QRgb*>(m_prev.constScanLine(y));

        for (int x = 0; x < w; ++x) {
            const int r = static_cast<int>(a * qRed(cur[x])   + b * qRed(prev[x])   + 0.5);
            const int g = static_cast<int>(a * qGreen(cur[x]) + b * qGreen(prev[x]) + 0.5);
            const int bl= static_cast<int>(a * qBlue(cur[x])  + b * qBlue(prev[x])  + 0.5);
            cur[x] = qRgba(std::clamp(r, 0, 255),
                           std::clamp(g, 0, 255),
                           std::clamp(bl, 0, 255),
                           qAlpha(cur[x]));
        }
    }

    m_prev = image.copy();
}

void AntiflickerPostProcessor::reset()
{
    m_prev = QImage{};
}

QString AntiflickerPostProcessor::name() const
{
    return QStringLiteral("Antiflicker");
}

void AntiflickerPostProcessor::setAlpha(double alpha)
{
    if (alpha <= 0.0 || alpha > 1.0)
        throw std::invalid_argument("AntiflickerPostProcessor: alpha must be in (0, 1]");
    m_alpha = alpha;
}

double AntiflickerPostProcessor::alpha() const noexcept
{
    return m_alpha;
}

} // namespace ntcap