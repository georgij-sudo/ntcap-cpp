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

    /** Текущий масштаб. */
    qreal zoom() const noexcept;

    /** Программная установка масштаба относительно центра виджета. */
    void setZoom(qreal zoom);

public slots:
    void refresh();

protected:
    void paintEvent(QPaintEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void rebuildColorTable();
    void rebuildImage();

    /** Преобразует координаты виджета → координаты поля. */
    QPointF widgetToField(const QPointF& widgetPos) const;

    /** Зум с якорной точкой (в координатах виджета). */
    void zoomAt(const QPointF& anchor, qreal newZoom);

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
    /** Масштаб: 1.0 = 1 клетка = 1 пиксель. */
    qreal   m_zoom = 1.0;

    /**
     * Смещение: координата поля, соответствующая
     * левому верхнему углу виджета.
     */
    QPointF m_offset{0.0, 0.0};

    // ── Панорамирование мышью ───────────────────────────
    bool    m_panning = false;
    QPointF m_panLastPos;
};

} // namespace ntcap