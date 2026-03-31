#include "CellularCanvas.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <cstring>
#include <cmath>
#include <algorithm>

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

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    m_colorMap[0] = qRgb(255, 255, 255);
    m_colorMap[1] = qRgb(  0,   0,   0);

    rebuildColorTable();
    m_image.fill(0);
}

// ─── Источник данных ────────────────────────────────────────────────

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

// ─── Геттеры / сеттеры камеры ───────────────────────────────────────

qreal CellularCanvas::zoom() const noexcept
{
    return m_zoom;
}

void CellularCanvas::setPanStep(qreal pixels) noexcept
{
    m_panStep = pixels;
}

void CellularCanvas::setZoomFactor(qreal factor) noexcept
{
    m_zoomFactor = factor;
}

// ─── Слот: обновление ───────────────────────────────────────────────

void CellularCanvas::refresh()
{
    rebuildImage();
    update();
}

// ─── Публичные слоты камеры ─────────────────────────────────────────

void CellularCanvas::zoomIn()
{
    const QPointF center(width() / 2.0, height() / 2.0);
    zoomAt(center, m_zoom * m_zoomFactor);
    update();
}

void CellularCanvas::zoomOut()
{
    const QPointF center(width() / 2.0, height() / 2.0);
    zoomAt(center, m_zoom / m_zoomFactor);
    update();
}

void CellularCanvas::panLeft()
{
    m_offset.rx() -= m_panStep / m_zoom;
    update();
}

void CellularCanvas::panRight()
{
    m_offset.rx() += m_panStep / m_zoom;
    update();
}

void CellularCanvas::panUp()
{
    m_offset.ry() -= m_panStep / m_zoom;
    update();
}

void CellularCanvas::panDown()
{
    m_offset.ry() += m_panStep / m_zoom;
    update();
}

// ─── paintEvent ─────────────────────────────────────────────────────

void CellularCanvas::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    painter.fillRect(rect(), QColor(40, 40, 40));

    const QPointF topLeftField     = widgetToField({0.0, 0.0});
    const QPointF bottomRightField = widgetToField({
        static_cast<qreal>(width()),
        static_cast<qreal>(height())
    });

    const qreal srcLeft   = std::max(topLeftField.x(),     0.0);
    const qreal srcTop    = std::max(topLeftField.y(),     0.0);
    const qreal srcRight  = std::min(bottomRightField.x(), static_cast<qreal>(m_fieldWidth));
    const qreal srcBottom = std::min(bottomRightField.y(), static_cast<qreal>(m_fieldHeight));

    if (srcLeft >= srcRight || srcTop >= srcBottom) {
        return;
    }

    const QRectF sourceRect(srcLeft, srcTop,
                            srcRight - srcLeft,
                            srcBottom - srcTop);

    const QRectF targetRect(
        (srcLeft   - m_offset.x()) * m_zoom,
        (srcTop    - m_offset.y()) * m_zoom,
        (srcRight  - srcLeft)      * m_zoom,
        (srcBottom - srcTop)       * m_zoom
    );

    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.drawImage(targetRect, m_image, sourceRect);
}

// ─── wheelEvent: зум к курсору ──────────────────────────────────────

void CellularCanvas::wheelEvent(QWheelEvent* event)
{
    constexpr qreal wheelZoomBase = 1.1;

    const int degrees = event->angleDelta().y();
    if (degrees == 0) {
        event->ignore();
        return;
    }

    const qreal steps = degrees / 120.0;
    const qreal factor = std::pow(wheelZoomBase, steps);

    zoomAt(event->position(), m_zoom * factor);
    update();

    event->accept();
}

// ─── Панорамирование мышью (ЛКМ) ───────────────────────────────────

void CellularCanvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_panning    = true;
        m_panLastPos = event->position();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void CellularCanvas::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning) {
        const QPointF delta = event->position() - m_panLastPos;
        m_offset.rx() -= delta.x() / m_zoom;
        m_offset.ry() -= delta.y() / m_zoom;
        m_panLastPos = event->position();
        update();
        event->accept();
        return;
    }
    QWidget::mouseMoveEvent(event);
}

void CellularCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }
    QWidget::mouseReleaseEvent(event);
}

// ─── Приватные методы ───────────────────────────────────────────────

QPointF CellularCanvas::widgetToField(const QPointF& widgetPos) const
{
    return QPointF(widgetPos.x() / m_zoom + m_offset.x(),
                   widgetPos.y() / m_zoom + m_offset.y());
}

void CellularCanvas::zoomAt(const QPointF& anchor, qreal newZoom)
{
    const QPointF fieldAnchor = widgetToField(anchor);
    m_zoom = newZoom;
    m_offset.setX(fieldAnchor.x() - anchor.x() / m_zoom);
    m_offset.setY(fieldAnchor.y() - anchor.y() / m_zoom);
}

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