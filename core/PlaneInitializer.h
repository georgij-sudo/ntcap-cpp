#pragma once

#include "Planes.h"
#include <cstddef>
#include <cstdint>

namespace ntcap {

/**
 * @brief Stateless utilities for filling Planes content.
 *
 * All methods draw on top of existing data without clearing.
 */
class PlaneInitializer {
public:
    PlaneInitializer() = delete;

    /**
     * @brief Randomly set cells to @p value with given probability.
     *
     * Cells that do not hit the density check are left untouched.
     *
     * @param planes      modified in-place.
     * @param planeIndex  target plane.
     * @param density     probability [0, 1] that a cell gets @p value.
     * @param value       fill value.
     * @throws std::out_of_range      planeIndex >= numPlanes.
     * @throws std::invalid_argument  density outside [0, 1].
     */
    static void randomFill(Planes& planes, size_t planeIndex,
                            double density, uint8_t value = 1);

    /**
     * @brief Random-fill every plane.
     */
    static void randomFillAll(Planes& planes,
                               double density, uint8_t value = 1);

    /**
     * @brief Draw a centred rectangle without clearing.
     * @param sqWidth   rectangle width  (clamped to field width).
     * @param sqHeight  rectangle height (clamped to field height).
     * @throws std::out_of_range planeIndex >= numPlanes.
     */
    static void fillSquare(Planes& planes, size_t planeIndex,
                            size_t sqWidth, size_t sqHeight,
                            uint8_t value = 1);

    /**
     * @brief Draw a centred ellipse without clearing.
     * @param diamX  horizontal diameter (clamped to field width).
     * @param diamY  vertical diameter   (clamped to field height).
     * @throws std::out_of_range planeIndex >= numPlanes.
     */
    static void fillCircle(Planes& planes, size_t planeIndex,
                            size_t diamX, size_t diamY,
                            uint8_t value = 1);
};

} // namespace ntcap