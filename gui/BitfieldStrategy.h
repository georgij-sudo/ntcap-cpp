#pragma once

#include "DisplayStrategy.h"

namespace ntcap {

/**
 * @brief Битовое объединение плоскостей: value = plane0 + 2*plane1 + 4*plane2 + ...
 *
 * Если число плоскостей больше 8, результат берётся modulo 256 (младшие 8 бит).
 */
class BitfieldStrategy final : public DisplayStrategy {
public:
    BitfieldStrategy() = default;

    void render(const Planes& planes, QImage& image) const override;
    QString name() const override;
};

} // namespace ntcap