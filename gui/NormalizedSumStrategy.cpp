#include "NormalizedSumStrategy.h"

namespace ntcap {

void NormalizedSumStrategy::render(const Planes& planes, QImage& image) const
{
    const size_t w = planes.width();
    const size_t h = planes.height();
    const size_t numPlanes = planes.numPlanes();

    if (w == 0 || h == 0 || numPlanes == 0)
        return;

    const int iw = image.width();
    const int ih = image.height();
    if (static_cast<size_t>(iw) != w || static_cast<size_t>(ih) != h) {
        image = QImage(static_cast<int>(w),
                       static_cast<int>(h),
                       QImage::Format_Indexed8);
    }

    for (size_t y = 0; y < h; ++y) {
        uint8_t* dst = image.scanLine(static_cast<int>(y));
        for (size_t x = 0; x < w; ++x) {
            size_t sum = 0;
            const size_t idx = y * w + x;
            for (size_t p = 0; p < numPlanes; ++p) {
                const auto planeSpan = planes.view(p);
                if (planeSpan[idx])
                    ++sum;
            }
            const unsigned v = static_cast<unsigned>(
                (sum * 255 + numPlanes / 2) / numPlanes);
            dst[x] = static_cast<uint8_t>(v);
        }
    }
}

QString NormalizedSumStrategy::name() const
{
    return QStringLiteral("Normalized Sum");
}

} // namespace ntcap