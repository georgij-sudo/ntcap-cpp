#pragma once

#include "PlanesConverter.h"
#include <string>

namespace ntcap {

/**
 * @brief Converter for Plaintext (.cells) format from conwaylife.com.
 *
 * Format:
 *   - Lines starting with '!' are comments (skipped on read, preserved header on write).
 *   - 'O', 'o', '*' = alive cell (value 1).
 *   - '.' or any other character = dead cell (value 0).
 *   - Shorter rows are right-padded with dead cells.
 *
 * Only plane 0 is read/written — format does not support multiple planes.
 *
 * @throws PlanesConverterException on empty input or field-size mismatch.
 */
class PlaintextPlanesConverter final : public PlanesConverter {
public:
    /**
     * @brief Serialize plane 0 to Plaintext format.
     */
    void serialize(const Planes& planes, std::ostream& out) const override;

    /**
     * @brief Deserialize from Plaintext. Creates Planes(patW, patH, 1).
     */
    Planes deserialize(std::istream& in) const override;

    /**
     * @brief Parse from string literal, field = pattern size + margin.
     *
     * Usage:
     * @code
     *   auto p = PlaintextPlanesConverter::fromString(cellsText, 2, 10);
     * @endcode
     *
     * @param cells     .cells content (may include '!' comments).
     * @param numPlanes number of planes in result (default 1).
     * @param margin    padding around pattern on each side (default 0).
     */
    static Planes fromString(const std::string& cells,
                              size_t numPlanes = 1,
                              size_t margin = 0);

    /**
     * @brief Parse from string literal, stamp centered on given field.
     *
     * Usage:
     * @code
     *   auto p = PlaintextPlanesConverter::fromString(cellsText, 200, 200, 2);
     * @endcode
     *
     * @param cells       .cells content.
     * @param fieldWidth  target field width.
     * @param fieldHeight target field height.
     * @param numPlanes   number of planes in result (default 1).
     * @throws PlanesConverterException if pattern exceeds field dimensions.
     */
    static Planes fromString(const std::string& cells,
                              size_t fieldWidth,
                              size_t fieldHeight,
                              size_t numPlanes = 1);
};

} // namespace ntcap