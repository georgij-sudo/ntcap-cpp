#pragma once

#include <cstdint>
#include <vector>
#include <span>

namespace ntcap {

/**
 * The Planes class implements multiple planes of cells (vector of uint8_t).
 * Each cellular plane has the same dimensions as other planes.
 */
class Planes {
public:
    /**
     * Creates Planes with (widht, height) dimensions 
     * and numPlanes planes. Initializes each plane with 0.
     */
    Planes(size_t width, size_t height, size_t numPlanes=1);

public:
    /**
     * Returns cell at cellIndex from plane at planeIndex.
     */
    uint8_t operator()(size_t planeIndex, size_t cellIndex) const noexcept;

    /**
     * Returns view of a single plane with index planeIndex.
     *
     * @param planeIndex specified plane index.
     */
    std::span<const uint8_t> view(size_t planeIndex) const noexcept;

    /**
     * Returns raw view of planes packed in single dimension vector.
     */
    std::span<const uint8_t> view() const noexcept;

    /**
     * Returns m_data.data().
     */
    const uint8_t* data() const noexcept;

    /**
     * Returns m_data.data().
     */
    uint8_t* data() noexcept;

    /**
     * Returns plane width.
     */
    size_t width() const noexcept;

    /**
     * Returns plane height.
     */
    size_t height() const noexcept;

    /**
     * Returns plane size = width*height.
     */
    size_t size() const noexcept;

private:
    /**
     * Plane width.
     */
    const size_t m_width;
    /**
     * Plane size=width*height.
     */
    const size_t m_size;
    /**
     * Number of planes.
     */
    const size_t m_numPlanes;

    /**
     * Planes data packed into 1 dimension vector.
     * So that plane0=m_data[0:m_size-1],
     * plane1=m_data[m_size:2*m_size-1], etc.
     */
    std::vector<uint8_t> m_data;
};

} // namespace ntcap

