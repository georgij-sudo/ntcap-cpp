#pragma once

#include "DisplayStrategy.h"

namespace ntcap {

/**
 * @brief Нормализует сумму плоскостей в диапазон 0..255:
 * value = round(sum_i * 255 / numPlanes)
 */
class NormalizedSumStrategy final : public DisplayStrategy {
public:
    NormalizedSumStrategy() = default;

    void render(const Planes& planes, QImage& image) const override;
    QString name() const override;
};

} // namespace ntcap