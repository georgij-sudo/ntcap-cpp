#include "TorusTopology.h"

#include <cstdint>
#include <stdexcept>

/**
 * VIBE CODE ALERT
 */

namespace ntcap {

TorusTopology::TorusTopology(size_t width, size_t height)
    : m_width(width)
    , m_height(height)
{
    if (width == 0) {
        throw std::out_of_range { "TorusTopology width is out of range: " + std::to_string(width) };
    }

    if (height == 0) {
        throw std::out_of_range { "TorusTopology height is out of range" + std::to_string(height) };
    }
}

UCoord2D TorusTopology::normalize(const Coord2D& coord)
{
    // Модульная арифметика: приведение знаковых координат
    // к диапазону [0, m_width) и [0, m_height) соответственно.
    // Оператор % в C++ для отрицательных чисел может вернуть
    // отрицательный остаток, поэтому корректируем вручную.

    auto mod = [](int64_t value, size_t divisor) -> size_t {
        const auto d = static_cast<int64_t>(divisor);
        return static_cast<size_t>(((value % d) + d) % d);
    };

    return UCoord2D {
        mod(coord.x, m_width),
        mod(coord.y, m_height)
    };
}

} // namespace ntcap
