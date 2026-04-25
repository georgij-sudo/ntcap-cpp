#include "ColorMapPresets.h"

#include <QtMath>
#include <QColor>

namespace ntcap {

QList<ColorMapPreset> ColorMapPresets::all()
{
    return {
        { QStringLiteral("Black & White (2)"),  binary()      },
        { QStringLiteral("Alive (2)"), alive()       },
        { QStringLiteral("Espresso (2)"),       espresso()    },
        { QStringLiteral("Alive w/ History (4)"),     aliveHistory() },
        { QStringLiteral("Grayscale (16)"),     grayscale16() },
        { QStringLiteral("Grayscale (256)"),    grayscale()   },
        { QStringLiteral("Heat (256)"),         heat()        },
        { QStringLiteral("Rainbow (256)"),      rainbow()     },
    };
}

ColorMap ColorMapPresets::binary()
{
    ColorMap m;
    m[0] = qRgb(0, 0, 0);
    m[1] = qRgb(255, 255, 255);
    return m;
}

ColorMap ColorMapPresets::alive()
{
    ColorMap m;
    m[0] = qRgb(0, 0, 0);
    m[1] = qRgb(0, 255, 0);
    return m;
}

ColorMap ColorMapPresets::espresso() {
    ColorMap m;
    m[0] = qRgb(0x3E, 0x27, 0x23);
    m[1] = qRgb(0xF0, 0x66, 0x0A);
    return m;
}

ColorMap ColorMapPresets::aliveHistory() {
    ColorMap m;
    m[0] = qRgb(0x00, 0x00, 0x00);
    m[1] = qRgb(0x00, 0xFF, 0x00);
    m[2] = qRgb(0xFF, 0x00, 0x00);
    m[3] = qRgb(0x00, 0x00, 0xFF);
    return m;
}

ColorMap ColorMapPresets::grayscale16() {
    ColorMap m;
    for (int i = 0; i < 16; ++i) {
        int v = i * 255 / 15;   // 0 → 0, 15 → 255
        m[static_cast<uint8_t>(i)] = qRgb(v, v, v);
    }
    return m;
}

ColorMap ColorMapPresets::grayscale()
{
    ColorMap m;
    for (int i = 0; i < 256; ++i)
        m[static_cast<uint8_t>(i)] = qRgb(i, i, i);
    return m;
}

ColorMap ColorMapPresets::heat()
{
    ColorMap m;
    for (int i = 0; i < 256; ++i) {
        double t = i / 255.0;
        int r = qBound(0, static_cast<int>(255 * qMin(1.0, t * 3.0)), 255);
        int g = qBound(0, static_cast<int>(255 * qMin(1.0, qMax(0.0, t - 0.33) * 3.0)), 255);
        int b = qBound(0, static_cast<int>(255 * qMin(1.0, qMax(0.0, t - 0.66) * 3.0)), 255);
        m[static_cast<uint8_t>(i)] = qRgb(r, g, b);
    }
    return m;
}

ColorMap ColorMapPresets::rainbow()
{
    ColorMap m;
    for (int i = 0; i < 256; ++i) {
        double hue = (i / 255.0) * 360.0;
        QColor c = QColor::fromHsvF(hue / 360.0, 1.0, 1.0);
        m[static_cast<uint8_t>(i)] = c.rgb();
    }
    return m;
}

} // namespace ntcap
