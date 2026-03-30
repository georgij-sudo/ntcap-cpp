#include "CellularCanvas.h"

#include <QPainter>
#include <QPaintEvent>
#include <cstring>

namespace ntcap {

// ─── Конструктор ────────────────────────────────────────────────────

CellularCanvas::CellularCanvas(size_t fieldWidth,
                                 size_t fieldHeight,
                                 QWidget* parent)
    : QWidget(parent)
    , m_fieldWidth(fieldWidth)
    , m_fieldHeight(fieldHeight)
    , m_image(static_cast<int>(fieldWidth),
              static_cast<int>(fieldHeight),
              QImage::Format_Indexed8)
{
    setFixedSize(static_cast<int>(fieldWidth),
                 static_cast<int>(fieldHeight));

    // Цвета по умолчанию: 0 → белый, 1 → чёрный.
    m_colorMap[0] = qRgb(255, 255, 255);
    m_colorMap[1] = qRgb(  0,   0,   0);

    rebuildColorTable();
    m_image.fill(0);
}

// ─── Установка источника данных ─────────────────────────────────────

void CellularCanvas::setPlanes(const Planes* planes,
                                 size_t planeIndex) noexcept
{
    m_planes     = planes;
    m_planeIndex = planeIndex;
}

// ─── Цветовая карта ─────────────────────────────────────────────────

void CellularCanvas::setColorMap(const QMap<uint8_t, QRgb>& colorMap) noexcept
{
    m_colorMap = colorMap;
    rebuildColorTable();
}

void CellularCanvas::setDefaultColor(QRgb color) noexcept
{
    m_defaultColor = color;
    rebuildColorTable();
}

// ─── Слот: обновление ───────────────────────────────────────────────

void CellularCanvas::refresh()
{
    rebuildImage();
    update();
}

// ─── paintEvent ─────────────────────────────────────────────────────

void CellularCanvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    const QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect.topLeft(), m_image, dirtyRect);
}

// ─── Построение цветовой таблицы QImage из QMap ─────────────────────

void CellularCanvas::rebuildColorTable()
{
    QList<QRgb> colorTable;
    colorTable.reserve(256);

    for (int i = 0; i < 256; ++i) {
        const auto state = static_cast<uint8_t>(i);
        colorTable.append(m_colorMap.value(state, m_defaultColor));
    }

    m_image.setColorTable(colorTable);
}

// ─── Построение QImage из Planes ────────────────────────────────────

void CellularCanvas::rebuildImage()
{
    if (!m_planes) {
        m_image.fill(0);
        return;
    }

    const std::span<const uint8_t> plane = m_planes->view(m_planeIndex);
    const int w = static_cast<int>(m_fieldWidth);
    const int h = static_cast<int>(m_fieldHeight);

    for (int y = 0; y < h; ++y) {
        uint8_t*       dst = m_image.scanLine(y);
        const uint8_t* src = plane.data() + static_cast<size_t>(y) * m_fieldWidth;
        std::memcpy(dst, src, static_cast<size_t>(w));
    }
}

} // namespace ntcap