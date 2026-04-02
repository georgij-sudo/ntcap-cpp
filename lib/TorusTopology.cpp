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

} // namespace ntcap
