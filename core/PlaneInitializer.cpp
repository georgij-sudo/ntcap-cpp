#include "PlaneInitializer.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <random>
#include <stdexcept>

namespace ntcap {

static void validatePlaneIndex(const Planes& planes, size_t idx)
{
    if (idx >= planes.numPlanes())
        throw std::out_of_range(
            "planeIndex " + std::to_string(idx) +
            " >= numPlanes " + std::to_string(planes.numPlanes()));
}

static void validateDensity(double density)
{
    if (density < 0.0 || density > 1.0)
        throw std::invalid_argument(
            "density must be in [0, 1], got " + std::to_string(density));
}

static void validateDistribution(const DistributionParams& dist)
{
    if (auto* u = std::get_if<UniformParams>(&dist)) {
        if (u->minValue > u->maxValue)
            throw std::invalid_argument(
                "Uniform: minValue (" + std::to_string(u->minValue) +
                ") > maxValue (" + std::to_string(u->maxValue) + ")");
    } else if (auto* n = std::get_if<NormalParams>(&dist)) {
        if (n->stddev <= 0.0)
            throw std::invalid_argument(
                "Normal: stddev must be > 0, got " +
                std::to_string(n->stddev));
    }
}

static uint8_t* planeData(Planes& planes, size_t planeIndex) noexcept
{
    return planes.data() + planeIndex * planes.size();
}

static void fillPlane(uint8_t* dst, size_t count, double density,
                      const DistributionParams& dist, std::mt19937& rng)
{
    std::bernoulli_distribution gate(density);

    std::visit([&](const auto& p) {
        using T = std::decay_t<decltype(p)>;

        if constexpr (std::is_same_v<T, BernoulliParams>) {
            for (size_t i = 0; i < count; ++i) {
                if (gate(rng))
                    dst[i] = p.value;
            }
        } else if constexpr (std::is_same_v<T, UniformParams>) {
            std::uniform_int_distribution<int> valueDist(p.minValue, p.maxValue);
            for (size_t i = 0; i < count; ++i) {
                if (gate(rng))
                    dst[i] = static_cast<uint8_t>(valueDist(rng));
            }
        } else if constexpr (std::is_same_v<T, NormalParams>) {
            std::normal_distribution<double> valueDist(p.mean, p.stddev);
            for (size_t i = 0; i < count; ++i) {
                if (gate(rng)) {
                    double raw = std::round(valueDist(rng));
                    double clamped = std::clamp(raw, 0.0, 255.0);
                    dst[i] = static_cast<uint8_t>(clamped);
                }
            }
        }
    }, dist);
}

void PlaneInitializer::randomFill(Planes& planes, size_t planeIndex,
                                   const RandomFillParams& params)
{
    validatePlaneIndex(planes, planeIndex);
    validateDensity(params.density);
    validateDistribution(params.distribution);

    if (params.density <= 0.0)
        return;

    std::mt19937 rng(params.seed);
    uint8_t* dst = planeData(planes, planeIndex);
    fillPlane(dst, planes.size(), params.density, params.distribution, rng);
}

void PlaneInitializer::randomFillAll(Planes& planes,
                                      const RandomFillParams& params)
{
    validateDensity(params.density);
    validateDistribution(params.distribution);

    if (params.density <= 0.0)
        return;

    std::mt19937 rng(params.seed);
    for (size_t p = 0; p < planes.numPlanes(); ++p) {
        uint8_t* dst = planeData(planes, p);
        fillPlane(dst, planes.size(), params.density, params.distribution, rng);
    }
}

void PlaneInitializer::fillSquare(Planes& planes, size_t planeIndex,
                                   size_t sqWidth, size_t sqHeight,
                                   uint8_t value)
{
    validatePlaneIndex(planes, planeIndex);

    const size_t fw = planes.width();
    const size_t fh = planes.height();

    sqWidth  = std::min(sqWidth,  fw);
    sqHeight = std::min(sqHeight, fh);

    if (sqWidth == 0 || sqHeight == 0)
        return;

    const size_t startX = (fw - sqWidth)  / 2;
    const size_t startY = (fh - sqHeight) / 2;

    uint8_t* dst = planeData(planes, planeIndex);

    for (size_t y = startY; y < startY + sqHeight; ++y)
        for (size_t x = startX; x < startX + sqWidth; ++x)
            dst[y * fw + x] = value;
}

void PlaneInitializer::fillCircle(Planes& planes, size_t planeIndex,
                                   size_t diamX, size_t diamY,
                                   uint8_t value)
{
    validatePlaneIndex(planes, planeIndex);

    const size_t fw = planes.width();
    const size_t fh = planes.height();

    diamX = std::min(diamX, fw);
    diamY = std::min(diamY, fh);

    if (diamX == 0 || diamY == 0)
        return;

    const double cx = static_cast<double>(fw) / 2.0;
    const double cy = static_cast<double>(fh) / 2.0;
    const double rx = static_cast<double>(diamX) / 2.0;
    const double ry = static_cast<double>(diamY) / 2.0;

    uint8_t* dst = planeData(planes, planeIndex);

    for (size_t y = 0; y < fh; ++y) {
        const double dy = (static_cast<double>(y) + 0.5 - cy) / ry;
        for (size_t x = 0; x < fw; ++x) {
            const double dx = (static_cast<double>(x) + 0.5 - cx) / rx;
            if (dx * dx + dy * dy <= 1.0)
                dst[y * fw + x] = value;
        }
    }
}

} // namespace ntcap