#include "PlaintextPlanesConverter.h"

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace ntcap {

namespace {

struct ParsedPattern {
    size_t width  = 0;
    size_t height = 0;
    std::vector<std::string> rows;
};

bool isAlive(char c) noexcept
{
    return c == 'O' || c == 'o' || c == '*';
}

ParsedPattern parseLines(std::istream& in)
{
    ParsedPattern pat;
    std::string line;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (!line.empty() && line[0] == '!')
            continue;

        pat.width = std::max(pat.width, line.size());
        pat.rows.push_back(std::move(line));
    }

    while (!pat.rows.empty() && pat.rows.back().empty())
        pat.rows.pop_back();

    pat.height = pat.rows.size();
    return pat;
}

void stampPattern(const ParsedPattern& pat, Planes& planes,
                  size_t ox, size_t oy)
{
    uint8_t* dst    = planes.data();
    size_t   stride = planes.width();

    for (size_t y = 0; y < pat.height; ++y) {
        const auto& row = pat.rows[y];
        size_t fy = oy + y;
        if (fy >= planes.height())
            break;

        for (size_t x = 0; x < row.size(); ++x) {
            size_t fx = ox + x;
            if (fx >= planes.width())
                break;

            if (isAlive(row[x]))
                dst[fy * stride + fx] = 1;
        }
    }
}

} // anonymous namespace

// ── PlanesConverter interface ───────────────────────────

void PlaintextPlanesConverter::serialize(const Planes& planes,
                                          std::ostream& out) const
{
    out << "! " << planes.width() << "x" << planes.height() << '\n';

    auto view = planes.view(0);
    size_t W  = planes.width();
    size_t H  = planes.height();

    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x)
            out << (view[y * W + x] != 0 ? 'O' : '.');
        out << '\n';
    }
}

Planes PlaintextPlanesConverter::deserialize(std::istream& in) const
{
    auto pat = parseLines(in);

    if (pat.width == 0 || pat.height == 0)
        throw PlanesConverterException("Empty pattern",
                                        "No cell data found in .cells input");

    Planes planes(pat.width, pat.height, 1);
    stampPattern(pat, planes, 0, 0);
    return planes;
}

// ── Static convenience methods ──────────────────────────

Planes PlaintextPlanesConverter::fromString(const std::string& cells,
                                             size_t numPlanes,
                                             size_t margin)
{
    std::istringstream ss(cells);
    auto pat = parseLines(ss);

    if (pat.width == 0 || pat.height == 0)
        throw PlanesConverterException("Empty pattern",
                                        "No cell data found in .cells string");

    size_t fw = pat.width  + 2 * margin;
    size_t fh = pat.height + 2 * margin;

    Planes planes(fw, fh, numPlanes);
    stampPattern(pat, planes, margin, margin);
    return planes;
}

Planes PlaintextPlanesConverter::fromString(const std::string& cells,
                                             size_t fieldWidth,
                                             size_t fieldHeight,
                                             size_t numPlanes)
{
    std::istringstream ss(cells);
    auto pat = parseLines(ss);

    if (pat.width == 0 || pat.height == 0)
        throw PlanesConverterException("Empty pattern",
                                        "No cell data found in .cells string");

    if (pat.width > fieldWidth || pat.height > fieldHeight)
        throw PlanesConverterException(
            "Pattern too large",
            "Pattern is " + std::to_string(pat.width) + "x"
            + std::to_string(pat.height) + " but field is "
            + std::to_string(fieldWidth) + "x"
            + std::to_string(fieldHeight));

    Planes planes(fieldWidth, fieldHeight, numPlanes);

    size_t ox = (fieldWidth  - pat.width)  / 2;
    size_t oy = (fieldHeight - pat.height) / 2;

    stampPattern(pat, planes, ox, oy);
    return planes;
}

} // namespace ntcap