#include "GrayscalePostProcessor.h"

#include <QRgb>
#include <stdexcept>

namespace ntcap {

void GrayscalePostProcessor::process(QImage& image)
{
    if (image.format() != QImage::Format_ARGB32)
        throw std::invalid_argument("GrayscalePostProcessor: expected ARGB32");

    const int w = image.width();
    const int h = image.height();

    for (int y = 0; y < h; ++y) {
        auto* line = reinterpret_cast<QRgb*>(image.scanLine(y));
        for (int x = 0; x < w; ++x) {
            const QRgb px = line[x];
            const int gray = (299 * qRed(px) + 587 * qGreen(px) + 114 * qBlue(px) + 500) / 1000;
            line[x] = qRgba(gray, gray, gray, qAlpha(px));
        }
    }
}

void GrayscalePostProcessor::reset()
{
}

QString GrayscalePostProcessor::name() const
{
    return QStringLiteral("Grayscale");
}

} // namespace ntcap