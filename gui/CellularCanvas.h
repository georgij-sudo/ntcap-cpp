#pragma once

#include<QWidget>
#include <QImage>
#include <QMap>
#include <QRgb>

#include "Planes.h"

namespace ntcap {

class CellularCanvas final : public QWidget {
    Q_OBJECT

public:
    explicit CellularCanvas(size_t fieldWidth,
                            size_t fieldHeight,
                            QWidget* parent = nullptr);

    void setPlanes(const Planes* planes, size_t planeIndex = 0) noexcept;

    /**
     * Задаёт таблицу соответствия состояний и цветов.
     * Состояния, отсутствующие в таблице, отображаются как defaultColor.
     */
    void setColorMap(const QMap<uint8_t, QRgb>& colorMap) noexcept;

    /**
     * Цвет для состояний, не указанных в colorMap.
     * По умолчанию — белый.
     */
    void setDefaultColor(QRgb color) noexcept;

public slots:
    void refresh();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void rebuildColorTable();
    void rebuildImage();

    const size_t  m_fieldWidth;
    const size_t  m_fieldHeight;
    const Planes* m_planes     = nullptr;
    size_t        m_planeIndex = 0;

    QMap<uint8_t, QRgb> m_colorMap;
    QRgb                m_defaultColor = qRgb(255, 255, 255);
    QImage              m_image;
};

} // namespace ntcap