#pragma once

#include "DisplayStrategy.h"

namespace ntcap {

/**
 * @brief Суммирует значения по плоскостям и обрезает значение до 255.
 */
class SaturatingSumStrategy final : public DisplayStrategy {
public:
    SaturatingSumStrategy() = default;

    void render(const Planes& planes, QImage& image) const override;
    QString name() const override;
};

} // namespace ntcap