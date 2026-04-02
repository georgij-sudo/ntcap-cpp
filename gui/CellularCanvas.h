#pragma once
#include <QWidget>
#include <QImage>
#include <QMap>
#include <QRgb>
#include <QPointF>
#include "Planes.h"
#include "TripleBuffer.h"

namespace ntcap {

class CellularCanvas final : public QWidget {
    Q_OBJECT
public:
    explicit CellularCanvas(QWidget* parent = nullptr);
    CellularCanvas(size_t fieldWidth, size_t fieldHeight,
                   QWidget* parent = nullptr);

    void setFieldSize(size_t width, size_t height);

    void setTripleBuffer(TripleBuffer<Planes>* tb,
                         size_t planeIndex = 0) noexcept;

    void setColorMap(const QMap<uint8_t, QRgb>& colorMap) noexcept;
    void setDefaultColor(QRgb color) noexcept;

    qreal   zoom()   const noexcept;
    QPointF offset() const noexcept;
    void setPanStep(qreal pixels) noexcept;
    void setZoomFactor(qreal factor) noexcept;

    QSize sizeHint()        const override;
    QSize minimumSizeHint() const override;

public slots:
    void refresh();
    void zoomIn();
    void zoomOut();
    void panLeft();
    void panRight();
    void panUp();
    void panDown();
    void zoomAt(const QPointF& widgetAnchor, qreal newZoom);
    void panByPixels(qreal dx, qreal dy);
    void fitToWindow();

protected:
    void paintEvent(QPaintEvent* event)   override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void    initDefaults();
    void    rebuildColorTable();
    void    rebuildImage();
    QPointF widgetToField(const QPointF& widgetPos) const;

    size_t                m_fieldWidth   = 0;
    size_t                m_fieldHeight  = 0;
    TripleBuffer<Planes>* m_triple       = nullptr;
    size_t                m_planeIndex   = 0;

    QMap<uint8_t, QRgb>   m_colorMap;
    QRgb                  m_defaultColor = qRgb(255, 255, 255);
    QImage                m_image;

    qreal   m_zoom       = 1.0;
    QPointF m_offset     = {0.0, 0.0};
    qreal   m_panStep    = 20.0;
    qreal   m_zoomFactor = 1.25;
    bool    m_fitPending = true;
};

} // namespace ntcap