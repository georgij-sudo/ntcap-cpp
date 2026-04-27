#include "Planes.h"

#include <stdexcept>

/**
 * VIBE CODE ALERT
 */

namespace ntcap {

Planes::Planes(size_t width, size_t height, size_t numPlanes)
    : m_width(width)
    , m_size(width * height)
    , m_numPlanes(numPlanes)
    , m_data(width * height * numPlanes, 0)
{
    if (width <= 0) {
        throw std::out_of_range { "Planes width is out of range: " + std::to_string(width) };
    }

    if (height <= 0) {
        throw std::out_of_range { "Planes height is out of range: " + std::to_string(height) };
    }

    if (numPlanes <= 0) {
        throw std::out_of_range { "Planes numPlanes is out of range: " + std::to_string(numPlanes) };
    }
}

uint8_t Planes::operator()(size_t planeIndex, size_t cellIndex) const noexcept
{
    return m_data[planeIndex * m_size + cellIndex];
}

std::span<const uint8_t> Planes::view(size_t planeIndex) const noexcept
{
    return std::span<const uint8_t>(m_data.data() + planeIndex * m_size, m_size);
}

std::span<const uint8_t> Planes::view() const noexcept
{
    return std::span<const uint8_t>(m_data.data(), m_data.size());
}

const uint8_t* Planes::data() const noexcept
{
    return m_data.data();
}

uint8_t* Planes::data() noexcept
{
    return m_data.data();
}

size_t Planes::width() const noexcept
{
    return m_width;
}

size_t Planes::height() const noexcept
{
    return m_size / m_width;
}

size_t Planes::size() const noexcept
{
    return m_size;
}

size_t Planes::numPlanes() const noexcept {
    return m_numPlanes;
}

void Planes::swapPlanes(size_t a, size_t b)
{
    if (a >= m_numPlanes)
        throw std::out_of_range(
            "Planes::swapPlanes: index a (" + std::to_string(a)
            + ") >= numPlanes (" + std::to_string(m_numPlanes) + ")");
    if (b >= m_numPlanes)
        throw std::out_of_range(
            "Planes::swapPlanes: index b (" + std::to_string(b)
            + ") >= numPlanes (" + std::to_string(m_numPlanes) + ")");
    if (a == b)
        return;

    uint8_t* pa = m_data.data() + a * m_size;
    uint8_t* pb = m_data.data() + b * m_size;
    std::swap_ranges(pa, pa + m_size, pb);
}

} // namespace ntcap
