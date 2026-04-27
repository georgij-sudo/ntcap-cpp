#include "SinglePlaneStrategy.h"
#include <cstring>

namespace ntcap {

SinglePlaneStrategy::SinglePlaneStrategy(size_t planeIndex) noexcept
    : m_planeIndex(planeIndex)
{}

void SinglePlaneStrategy::render(const Planes& planes, QImage& image) const
{
    const size_t w = planes.width();
    const size_t h = planes.height();
    const size_t numPlanes = planes.numPlanes();

    if (w == 0 || h == 0)
        return;

    const size_t planeIdx = (m_planeIndex < numPlanes) ? m_planeIndex : 0;
    const std::span<const uint8_t> plane = planes.view(planeIdx);

    const int iw = image.width();
    const int ih = image.height();
    if (static_cast<size_t>(iw) != w || static_cast<size_t>(ih) != h) {
        image = QImage(static_cast<int>(w),
                       static_cast<int>(h),
                       QImage::Format_Indexed8);
    }

    for (size_t y = 0; y < h; ++y) {
        uint8_t* dst = image.scanLine(static_cast<int>(y));
        const uint8_t* src = plane.data() + y * w;
        std::memcpy(dst, src, w);
    }
}

QString SinglePlaneStrategy::name() const
{
    return QStringLiteral("Plane %1").arg(static_cast<qsizetype>(m_planeIndex));
}

size_t SinglePlaneStrategy::planeIndex() const noexcept
{
    return m_planeIndex;
}

} // namespace ntcap