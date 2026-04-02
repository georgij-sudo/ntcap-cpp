#include "CellularCanvas.h"
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <cstring>
#include <cmath>
#include <algorithm>

namespace ntcap {

CellularCanvas::CellularCanvas(QWidget* parent)
    : QWidget(parent)
{
    initDefaults();
}

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
    initDefaults();
    m_image.fill(0);
}

void CellularCanvas::initDefaults()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    m_colorMap[0] = qRgb(255, 255, 255);
    m_colorMap[1] = qRgb(  0,   0,   0);
    rebuildColorTable();
}

void CellularCanvas::setFieldSize(size_t width, size_t height)
{
    if (m_fieldWidth == width && m_fieldHeight == height)
        return;
    m_fieldWidth  = width;
    m_fieldHeight = height;
    m_image = QImage(static_cast<int>(width),
                     static_cast<int>(height),
                     QImage::Format_Indexed8);
    rebuildColorTable();
    m_image.fill(0);
    m_fitPending = true;
    if (width > 0 && height > 0)
        fitToWindow();
    update();
}

void CellularCanvas::setTripleBuffer(TripleBuffer<Planes>* tb,
                                     size_t planeIndex) noexcept
{
    m_triple     = tb;
    m_planeIndex = planeIndex;
}

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

qreal   CellularCanvas::zoom()   const noexcept { return m_zoom;   }
QPointF CellularCanvas::offset() const noexcept  { return m_offset; }
void CellularCanvas::setPanStep(qreal v) noexcept    { m_panStep = v;   }
void CellularCanvas::setZoomFactor(qreal v) noexcept  { m_zoomFactor = v; }

QSize CellularCanvas::sizeHint() const        { return {640, 480}; }
QSize CellularCanvas::minimumSizeHint() const  { return {120, 90};  }

void CellularCanvas::refresh()
{
    if (!m_triple->consume())
        return;
    rebuildImage();
    update();
}

void CellularCanvas::zoomIn()
{
    const QPointF center(width() / 2.0, height() / 2.0);
    zoomAt(center, m_zoom * m_zoomFactor);
}

void CellularCanvas::zoomOut()
{
    const QPointF center(width() / 2.0, height() / 2.0);
    zoomAt(center, m_zoom / m_zoomFactor);
}

void CellularCanvas::panLeft()  { panByPixels( m_panStep, 0.0); }
void CellularCanvas::panRight() { panByPixels(-m_panStep, 0.0); }
void CellularCanvas::panUp()    { panByPixels(0.0,  m_panStep); }
void CellularCanvas::panDown()  { panByPixels(0.0, -m_panStep); }

void CellularCanvas::zoomAt(const QPointF& widgetAnchor, qreal newZoom)
{
    const QPointF fieldAnchor = widgetToField(widgetAnchor);
    m_zoom = newZoom;
    m_offset.setX(fieldAnchor.x() - widgetAnchor.x() / m_zoom);
    m_offset.setY(fieldAnchor.y() - widgetAnchor.y() / m_zoom);
    update();
}

void CellularCanvas::panByPixels(qreal dx, qreal dy)
{
    m_offset.rx() -= dx / m_zoom;
    m_offset.ry() -= dy / m_zoom;
    update();
}

void CellularCanvas::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_fitPending && m_fieldWidth > 0 && m_fieldHeight > 0)
        fitToWindow();
    update();
}

void CellularCanvas::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.fillRect(rect(), QColor(40, 40, 40));

    if (m_fieldWidth == 0 || m_fieldHeight == 0)
        return;

    const QPointF topLeftField     = widgetToField({0.0, 0.0});
    const QPointF bottomRightField = widgetToField({
        static_cast<qreal>(width()),
        static_cast<qreal>(height())
    });

    const qreal srcLeft   = std::max(topLeftField.x(),     0.0);
    const qreal srcTop    = std::max(topLeftField.y(),     0.0);
    const qreal srcRight  = std::min(bottomRightField.x(),
                                     static_cast<qreal>(m_fieldWidth));
    const qreal srcBottom = std::min(bottomRightField.y(),
                                     static_cast<qreal>(m_fieldHeight));

    if (srcLeft >= srcRight || srcTop >= srcBottom)
        return;

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

void CellularCanvas::fitToWindow()
{
    const qreal fw = static_cast<qreal>(m_fieldWidth);
    const qreal fh = static_cast<qreal>(m_fieldHeight);
    const qreal ww = static_cast<qreal>(width());
    const qreal wh = static_cast<qreal>(height());

    if (fw <= 0.0 || fh <= 0.0 || ww <= 0.0 || wh <= 0.0)
        return;

    m_zoom = std::min(ww / fw, wh / fh);
    m_offset.setX(fw / 2.0 - ww / 2.0 / m_zoom);
    m_offset.setY(fh / 2.0 - wh / 2.0 / m_zoom);
    m_fitPending = false;
}

QPointF CellularCanvas::widgetToField(const QPointF& widgetPos) const
{
    return {widgetPos.x() / m_zoom + m_offset.x(),
            widgetPos.y() / m_zoom + m_offset.y()};
}

void CellularCanvas::rebuildColorTable()
{
    QList<QRgb> colorTable;
    colorTable.reserve(256);
    for (int i = 0; i < 256; ++i)
        colorTable.append(m_colorMap.value(static_cast<uint8_t>(i),
                                           m_defaultColor));
    if (!m_image.isNull())
        m_image.setColorTable(colorTable);
}

void CellularCanvas::rebuildImage()
{
    if (!m_triple || m_fieldWidth == 0 || m_fieldHeight == 0) {
        if (!m_image.isNull())
            m_image.fill(0);
        return;
    }

    const Planes& planes = m_triple->readBuffer();
    const std::span<const uint8_t> plane = planes.view(m_planeIndex);

    const int w = static_cast<int>(m_fieldWidth);
    const int h = static_cast<int>(m_fieldHeight);

    for (int y = 0; y < h; ++y) {
        uint8_t*       dst = m_image.scanLine(y);
        const uint8_t* src = plane.data()
                             + static_cast<size_t>(y) * m_fieldWidth;
        std::memcpy(dst, src, static_cast<size_t>(w));
    }
}

} // namespace ntcap