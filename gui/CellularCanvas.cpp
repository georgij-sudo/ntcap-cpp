#include "CellularCanvas.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QKeyEvent>
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

    // Фокус для приёма клавиатурных событий.
    setFocusPolicy(Qt::StrongFocus);

    // Отслеживание мыши для курсора (опционально).
    setMouseTracking(true);

    // Цвета по умолчанию.
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

// ─── Зум ────────────────────────────────────────────────────────────

qreal CellularCanvas::zoom() const noexcept
{
    return m_zoom;
}

void CellularCanvas::setZoom(qreal zoom)
{
    const QPointF center(width() / 2.0, height() / 2.0);
    zoomAt(center, zoom);
    update();
}

// ─── Слот: обновление ───────────────────────────────────────────────

void CellularCanvas::refresh()
{
    rebuildImage();
    update();
}

// ─── paintEvent ─────────────────────────────────────────────────────

void CellularCanvas::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);

    // Фон за пределами поля.
    painter.fillRect(rect(), QColor(40, 40, 40));

    // Трансформация: поле → виджет.
    // widgetPos = (fieldPos - offset) * zoom
    painter.translate(-m_offset.x() * m_zoom,
                      -m_offset.y() * m_zoom);
    painter.scale(m_zoom, m_zoom);

    // При зуме > 1 рисуем пиксели без сглаживания (чёткие клетки).
    // При зуме < 1 — тоже без, для скорости.
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);

    painter.drawImage(0, 0, m_image);
}

// ─── wheelEvent: зум к курсору ──────────────────────────────────────

void CellularCanvas::wheelEvent(QWheelEvent* event)
{
    constexpr qreal zoomFactor = 1.1;

    const int degrees = event->angleDelta().y();
    if (degrees == 0) {
        event->ignore();
        return;
    }

    // Количество «щелчков» (обычно 120 единиц = 1 щелчок).
    const qreal steps = degrees / 120.0;
    const qreal factor = std::pow(zoomFactor, steps);
    const qreal newZoom = m_zoom * factor;

    zoomAt(event->position(), newZoom);
    update();

    event->accept();
}

// ─── keyPressEvent: Ctrl+Plus / Ctrl+Minus ──────────────────────────

void CellularCanvas::keyPressEvent(QKeyEvent* event)
{
    constexpr qreal zoomFactor = 1.25;

    // ── Зум: Ctrl+Plus / Ctrl+Minus ────────────────────
    if (event->modifiers() & Qt::ControlModifier) {
        const QPointF center(width() / 2.0, height() / 2.0);

        if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
            zoomAt(center, m_zoom * zoomFactor);
            update();
            event->accept();
            return;
        }
        if (event->key() == Qt::Key_Minus) {
            zoomAt(center, m_zoom / zoomFactor);
            update();
            event->accept();
            return;
        }
    }

    // ── Панорамирование: стрелки ────────────────────────
    // Сдвиг в пикселях виджета за одно нажатие.
    constexpr qreal panPixels = 20.0;

    switch (event->key()) {
    case Qt::Key_Left:
        m_offset.rx() -= panPixels / m_zoom;
        update();
        event->accept();
        return;
    case Qt::Key_Right:
        m_offset.rx() += panPixels / m_zoom;
        update();
        event->accept();
        return;
    case Qt::Key_Up:
        m_offset.ry() -= panPixels / m_zoom;
        update();
        event->accept();
        return;
    case Qt::Key_Down:
        m_offset.ry() += panPixels / m_zoom;
        update();
        event->accept();
        return;
    default:
        break;
    }

    QWidget::keyPressEvent(event);
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

        // Сдвиг в координатах поля = сдвиг в пикселях / zoom.
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
    // widgetPos = (fieldPos - offset) * zoom
    // fieldPos  = widgetPos / zoom + offset
    return QPointF(widgetPos.x() / m_zoom + m_offset.x(),
                   widgetPos.y() / m_zoom + m_offset.y());
}

void CellularCanvas::zoomAt(const QPointF& anchor, qreal newZoom)
{
    // Точка поля под якорем до зума.
    const QPointF fieldAnchor = widgetToField(anchor);

    m_zoom = newZoom;

    // После смены zoom пересчитываем offset так, чтобы
    // fieldAnchor осталась под тем же пикселем виджета.
    // anchor = (fieldAnchor - newOffset) * newZoom
    // newOffset = fieldAnchor - anchor / newZoom
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