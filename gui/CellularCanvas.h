#pragma once

#include <QWidget>
#include <QImage>
#include "Planes.h"

namespace ntcap {

/**
 * Простой виджет-канвас для отображения  плоскости Planes.
 *
 * Виджет НЕ владеет Planes. Внешний код обязан гарантировать,
 * что указатель остаётся валидным на время использования.
 */
class CellularCanvas final : public QWidget {
    Q_OBJECT

public:
    /**
     * @param fieldWidth   ширина поля (в клетках = в пикселях).
     * @param fieldHeight  высота поля (в клетках = в пикселях).
     * @param parent       родительский виджет Qt.
     */
    explicit CellularCanvas(size_t fieldWidth,
                            size_t fieldHeight,
                            QWidget* parent = nullptr);

    /**
     * Устанавливает источник данных для отображения.
     *
     * @param planes      указатель на Planes (не принимает владение).
     * @param planeIndex  индекс плоскости, которую нужно рисовать.
     */
    void setPlanes(const Planes* planes, size_t planeIndex = 0) noexcept;

    /**
     * Перестраивает внутренний QImage из текущего состояния Planes
     * и планирует перерисовку. Вызывайте после каждого шага симуляции.
     */
    void refresh();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void rebuildImage();

    const size_t  m_fieldWidth;
    const size_t  m_fieldHeight;
    const Planes* m_planes     = nullptr;
    size_t        m_planeIndex = 0;

    /**
     * Внутренний буфер: Format_Indexed8, цветовая таблица {white, black}.
     * Копирует данные из Planes при вызове refresh(), поэтому
     * paintEvent() безопасен даже если Planes изменится между
     * вызовом update() и фактической отрисовкой.
     */
    QImage m_image;
};

} // namespace ntcap