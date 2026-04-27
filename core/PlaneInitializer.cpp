#include "PlaneInitializer.h"

#include <algorithm>
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

static uint8_t* planeData(Planes& planes, size_t planeIndex) noexcept
{
    return planes.data() + planeIndex * planes.size();
}

void PlaneInitializer::randomFill(Planes& planes, size_t planeIndex,
                                   double density, uint8_t value)
{
    validatePlaneIndex(planes, planeIndex);
    validateDensity(density);

    if (density <= 0.0)
        return;

    uint8_t* dst = planeData(planes, planeIndex);
    const size_t count = planes.size();

    if (density >= 1.0) {
        std::memset(dst, value, count);
        return;
    }

    std::mt19937 rng{std::random_device{}()};
    std::bernoulli_distribution dist(density);

    for (size_t i = 0; i < count; ++i) {
        if (dist(rng))
            dst[i] = value;
    }
}

void PlaneInitializer::randomFillAll(Planes& planes,
                                      double density, uint8_t value)
{
    for (size_t p = 0; p < planes.numPlanes(); ++p)
        randomFill(planes, p, density, value);
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