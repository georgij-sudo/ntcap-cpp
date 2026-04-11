#pragma once

#include <QString>
#include <QList>
#include <QPair>
#include <QMap>
#include <QRgb>
#include <cstdint>

namespace ntcap {

using ColorMap = QMap<uint8_t, QRgb>;

struct ColorMapPreset
{
    QString  name;
    ColorMap map;
};

class ColorMapPresets
{
public:
    static QList<ColorMapPreset> all();

private:
    static ColorMap binary();
    static ColorMap alive();
    static ColorMap espresso();
    static ColorMap grayscale16();
    static ColorMap grayscale();
    static ColorMap heat();
    static ColorMap rainbow();
};

} // namespace ntcap