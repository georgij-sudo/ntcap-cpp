#pragma once

#include "PlanesConverter.h"
#include <string>

namespace ntcap {

/**
 * @brief Converter for standard text-based Life RLE format.
 *
 * Header comment lines (optional, prefixed with '#'):
 *   #C — comment, #c — comment (alt), #N — pattern name,
 *   #O — author/origin, #P / #R — top-left coordinates, #r — rule (legacy).
 *
 * Dimension line (required):
 *   x = <width>, y = <height>[, rule = <rulestring>]
 *
 * Pattern body tags:
 *   b = dead (0), o = alive (1),
 *   x = marked alive 1 (2), y = marked alive 2 (3), z = marked alive 3 (4),
 *   $ = end of row, ! = end of pattern.
 *   Digits before a tag indicate run-length count.
 *
 * Only plane 0 is used. Cell values are 0..4.
 *
 * @throws PlanesConverterException on parse errors or invalid data.
 */
class LifeRlePlanesConverter final : public PlanesConverter {
public:
    /**
     * @brief Serialize plane 0 to text Life RLE format.
     */
    void serialize(const Planes& planes, std::ostream& out) const override;

    /**
     * @brief Deserialize from text Life RLE. Creates Planes(w, h, 1).
     */
    Planes deserialize(std::istream& in) const override;

    /**
     * @brief Parse from string, field = pattern size + margin.
     *
     * @param rle        Life RLE text.
     * @param numPlanes  number of planes (default 1).
     * @param margin     padding around pattern (default 0).
     */
    static Planes fromString(const std::string& rle,
                              size_t numPlanes = 1,
                              size_t margin = 0);

    /**
     * @brief Parse from string, stamp centered on given field.
     *
     * @param rle          Life RLE text.
     * @param fieldWidth   target field width.
     * @param fieldHeight  target field height.
     * @param numPlanes    number of planes (default 1).
     * @throws PlanesConverterException if pattern exceeds field.
     */
    static Planes fromString(const std::string& rle,
                              size_t fieldWidth,
                              size_t fieldHeight,
                              size_t numPlanes = 1);

    /**
     * @brief Check if stream looks like text Life RLE (not binary PRLE).
     *
     * Peeks at first 4 bytes. Returns true if they do NOT match "PRLE".
     * Stream position is restored.
     */
    static bool looksLikeTextRle(std::istream& in);

private:
    struct ParsedRle {
        size_t width  = 0;
        size_t height = 0;
        std::string rule;
        std::vector<uint8_t> cells;
    };

    static ParsedRle parseStream(std::istream& in);
    static uint8_t   tagToValue(char tag);
    static char      valueToTag(uint8_t val) noexcept;
    static void      stampOnPlanes(const ParsedRle& parsed, Planes& planes,
                                   size_t ox, size_t oy);
};

} // namespace ntcap