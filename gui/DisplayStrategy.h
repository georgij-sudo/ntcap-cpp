#pragma once

#include <QImage>
#include "Planes.h"
#include <memory>
#include <QString>

namespace ntcap {

/**
 * @brief Абстрактный интерфейс стратегии отображения плоскостей.
 *
 * Стратегия получает все плоскости (Planes) и должна заполнить QImage
 * индексированными значениями (0..255). Все реализованные стратегии
 * предполагают, что QImage уже создан в формате QImage::Format_Indexed8
 * нужного размера.
 */
class DisplayStrategy {
public:
    virtual ~DisplayStrategy() = default;

    /**
     * @brief Выполнить рендеринг в переданный QImage.
     * @param planes  источник плоскостей (read-only).
     * @param image   целевой изображение (Format_Indexed8), размеры совпадают
     *                с полем. Должно быть заполнено алгоритмом.
     */
    virtual void render(const Planes& planes, QImage& image) const = 0;

    /**
     * @brief Читабельное имя стратегии (используется в меню).
     */
    virtual QString name() const = 0;
};

using DisplayStrategyPtr = std::shared_ptr<DisplayStrategy>;

} // namespace ntcap