#pragma once

#include <QWidget>
#include <QImage>
#include <QMap>
#include <QRgb>
#include <QPointF>

#include "Planes.h"

namespace ntcap {

class CellularCanvas final : public QWidget {
    Q_OBJECT

public:
    explicit CellularCanvas(size_t fieldWidth,
                            size_t fieldHeight,
                            QWidget* parent = nullptr);

    void setPlanes(const Planes* planes, size_t planeIndex = 0) noexcept;
    void setColorMap(const QMap<uint8_t, QRgb>& colorMap) noexcept;
    void setDefaultColor(QRgb color) noexcept;

    qreal zoom() const noexcept;
    QPointF offset() const noexcept;

    void setPanStep(qreal pixels) noexcept;
    void setZoomFactor(qreal factor) noexcept;

public slots:
    void refresh();

    // ── Камера: клавиатура ──────────────────────────────
    void zoomIn();
    void zoomOut();
    void panLeft();
    void panRight();
    void panUp();
    void panDown();

    // ── Камера: мышь ────────────────────────────────────
    /** Зум с якорем в координатах виджета. */
    void zoomAt(const QPointF& widgetAnchor, qreal newZoom);

    /** Сдвиг камеры на delta пикселей виджета. */
    void panByPixels(qreal dx, qreal dy);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void rebuildColorTable();
    void rebuildImage();
    QPointF widgetToField(const QPointF& widgetPos) const;

    // ── Поле ────────────────────────────────────────────
    const size_t  m_fieldWidth;
    const size_t  m_fieldHeight;
    const Planes* m_planes     = nullptr;
    size_t        m_planeIndex = 0;

    // ── Цвета ───────────────────────────────────────────
    QMap<uint8_t, QRgb> m_colorMap;
    QRgb                m_defaultColor = qRgb(255, 255, 255);
    QImage              m_image;

    // ── Вьюпорт ─────────────────────────────────────────
    qreal   m_zoom       = 1.0;
    QPointF m_offset     = {0.0, 0.0};
    qreal   m_panStep    = 20.0;
    qreal   m_zoomFactor = 1.25;
};

} // namespace ntcap