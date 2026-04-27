#pragma once

#include "Planes.h"

#include <cstddef>
#include <cstdint>
#include <variant>

namespace ntcap {

/**
 * @brief Supported random-fill distribution types.
 */
enum class FillDistribution { Bernoulli, Uniform, Normal };

/**
 * @brief Parameters for Bernoulli distribution.
 *
 * Each cell that passes the density check receives the fixed @p value.
 */
struct BernoulliParams {
    uint8_t value = 1;
};

/**
 * @brief Parameters for Uniform distribution.
 *
 * Each cell that passes the density check receives a random value
 * drawn uniformly from [@p minValue, @p maxValue].
 */
struct UniformParams {
    uint8_t minValue = 0;
    uint8_t maxValue = 255;
};

/**
 * @brief Parameters for Normal (Gaussian) distribution.
 *
 * Each cell that passes the density check receives a value sampled
 * from N(@p mean, @p stddev), clamped to [0, 255].
 */
struct NormalParams {
    double mean   = 128.0;
    double stddev = 40.0;
};

using DistributionParams = std::variant<BernoulliParams, UniformParams, NormalParams>;

/**
 * @brief Full parameter set for random fill operations.
 */
struct RandomFillParams {
    double             density      = 0.5;
    uint32_t           seed         = 0;
    DistributionParams distribution = BernoulliParams{};
};

/**
 * @brief Stateless utilities for filling Planes content.
 *
 * All methods draw on top of existing data without clearing.
 */
class PlaneInitializer {
public:
    PlaneInitializer() = delete;

    /**
     * @brief Randomly fill a single plane according to @p params.
     *
     * A single std::mt19937 is created from @p params.seed.
     * Cells that do not pass the density check are left untouched.
     *
     * @param planes      modified in-place.
     * @param planeIndex  target plane.
     * @param params      fill parameters (density, seed, distribution).
     * @throws std::out_of_range      planeIndex >= numPlanes.
     * @throws std::invalid_argument  density outside [0, 1].
     * @throws std::invalid_argument  Uniform minValue > maxValue.
     * @throws std::invalid_argument  Normal stddev <= 0.
     */
    static void randomFill(Planes& planes, size_t planeIndex,
                           const RandomFillParams& params);

    /**
     * @brief Randomly fill every plane sequentially.
     *
     * A single std::mt19937 is created from @p params.seed and shared
     * across all planes, making the result fully reproducible.
     *
     * @param planes  modified in-place.
     * @param params  fill parameters (density, seed, distribution).
     * @throws std::invalid_argument  density outside [0, 1].
     * @throws std::invalid_argument  Uniform minValue > maxValue.
     * @throws std::invalid_argument  Normal stddev <= 0.
     */
    static void randomFillAll(Planes& planes,
                              const RandomFillParams& params);

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