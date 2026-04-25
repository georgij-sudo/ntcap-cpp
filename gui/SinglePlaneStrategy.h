#pragma once

#include "DisplayStrategy.h"

namespace ntcap {

/**
 * @brief Отображает одну конкретную плоскость как индексированные значения.
 */
class SinglePlaneStrategy final : public DisplayStrategy {
public:
    /**
     * @param planeIndex индекс плоскости (0..).
     */
    explicit SinglePlaneStrategy(size_t planeIndex) noexcept;

    void render(const Planes& planes, QImage& image) const override;
    QString name() const override;

    size_t planeIndex() const noexcept;

private:
    size_t m_planeIndex;
};

} // namespace ntcap