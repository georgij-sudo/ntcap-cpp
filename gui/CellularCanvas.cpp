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
    // Фиксируем размер виджета: 1 клетка = 1 пиксель.
    setFixedSize(static_cast<int>(fieldWidth),
                 static_cast<int>(fieldHeight));

    // Цветовая таблица: индекс 0 → белый (мёртвая), индекс 1 → чёрный (живая).
    QList<QRgb> colorTable;
    colorTable.reserve(256);
    colorTable.append(qRgb(255, 255, 255));  // 0 — dead
    colorTable.append(qRgb(  0,   0,   0));  // 1 — alive
    // На случай если в Planes окажутся значения > 1, заполним остаток чёрным.
    for (int i = 2; i < 256; ++i) {
        colorTable.append(qRgb(0, 0, 0));
    }
    m_image.setColorTable(colorTable);

    // Начальная заливка — всё белое (мёртвое).
    m_image.fill(0);
}

// ─── Установка источника данных ─────────────────────────────────────

void CellularCanvas::setPlanes(const Planes* planes,
                                 size_t planeIndex) noexcept
{
    m_planes     = planes;
    m_planeIndex = planeIndex;
}

// ─── Обновление изображения и запрос перерисовки ────────────────────

void CellularCanvas::refresh()
{
    rebuildImage();
    update();  // планирует paintEvent (асинхронно, не дублируется)
}

// ─── paintEvent ─────────────────────────────────────────────────────

void CellularCanvas::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    // Рисуем только ту область, которую Qt попросил обновить.
    const QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect.topLeft(), m_image, dirtyRect);
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

    // Копируем построчно, т.к. QImage может добавлять padding в конце scanline
    // (bytesPerLine() ≥ width для Format_Indexed8).
    for (int y = 0; y < h; ++y) {
        uint8_t*       dst = m_image.scanLine(y);
        const uint8_t* src = plane.data() + static_cast<size_t>(y) * m_fieldWidth;
        std::memcpy(dst, src, static_cast<size_t>(w));
    }
}

} // namespace ntcap