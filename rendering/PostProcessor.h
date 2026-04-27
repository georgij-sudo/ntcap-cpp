// gui/PostProcessor.h
#pragma once

#include <QImage>
#include <QString>

namespace ntcap {

/**
 * @brief Абстрактный интерфейс пост-обработки кадра.
 *
 * Работает с QImage в формате QImage::Format_ARGB32.
 * Реализации могут быть stateful (хранить буферы предыдущих кадров).
 */
class PostProcessor {
public:
    virtual ~PostProcessor() = default;

    /**
     * @brief Применить эффект к изображению (in-place).
     * @param image  изображение в формате ARGB32.
     */
    virtual void process(QImage& image) = 0;

    /**
     * @brief Сбросить внутреннее состояние (буферы, историю).
     */
    virtual void reset() = 0;

    /**
     * @brief Читабельное имя эффекта.
     */
    virtual QString name() const = 0;
};

} // namespace ntcap