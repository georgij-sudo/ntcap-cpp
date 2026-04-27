#include "LifeRlePlanesConverter.h"

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <cstring>

namespace ntcap {

namespace {

bool isHeaderComment(const std::string& line)
{
    return line.size() >= 2 && line[0] == '#';
}

bool isDimensionLine(const std::string& line)
{
    auto pos = line.find('x');
    if (pos == std::string::npos)
        return false;
    auto eq = line.find('=', pos);
    return eq != std::string::npos;
}

std::string trim(const std::string& s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return {};
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

} // anonymous namespace

uint8_t LifeRlePlanesConverter::tagToValue(char tag)
{
    switch (tag) {
    case 'b': return 0;
    case 'o': return 1;
    case 'x': return 2;
    case 'y': return 3;
    case 'z': return 4;
    default:
        throw PlanesConverterException(
            "Unknown RLE tag",
            std::string("Unexpected character '") + tag + "' in pattern body");
    }
}

char LifeRlePlanesConverter::valueToTag(uint8_t val) noexcept
{
    switch (val) {
    case 0:  return 'b';
    case 1:  return 'o';
    case 2:  return 'x';
    case 3:  return 'y';
    case 4:  return 'z';
    default: return 'o';
    }
}

bool LifeRlePlanesConverter::looksLikeTextRle(std::istream& in)
{
    char magic[4] = {};
    auto pos = in.tellg();
    in.read(magic, 4);
    in.clear();
    in.seekg(pos);
    return std::memcmp(magic, "PRLE", 4) != 0;
}

LifeRlePlanesConverter::ParsedRle
LifeRlePlanesConverter::parseStream(std::istream& in)
{
    ParsedRle result;
    std::string line;
    std::string body;
    bool headerFound = false;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (isHeaderComment(line))
            continue;

        if (!headerFound && isDimensionLine(line)) {
            auto xEq = line.find('=');
            if (xEq == std::string::npos)
                throw PlanesConverterException("Invalid header", line);

            auto comma1 = line.find(',', xEq);
            if (comma1 == std::string::npos)
                throw PlanesConverterException("Missing y in header", line);

            std::string wStr = trim(line.substr(xEq + 1, comma1 - xEq - 1));
            result.width = std::stoul(wStr);

            auto yEq = line.find('=', comma1);
            if (yEq == std::string::npos)
                throw PlanesConverterException("Missing y= in header", line);

            auto comma2 = line.find(',', yEq);
            std::string hStr;
            if (comma2 != std::string::npos) {
                hStr = trim(line.substr(yEq + 1, comma2 - yEq - 1));
                auto ruleEq = line.find('=', comma2);
                if (ruleEq != std::string::npos)
                    result.rule = trim(line.substr(ruleEq + 1));
            } else {
                hStr = trim(line.substr(yEq + 1));
            }
            result.height = std::stoul(hStr);

            headerFound = true;
            continue;
        }

        if (headerFound)
            body += line;
    }

    if (!headerFound)
        throw PlanesConverterException("No header line found",
                                        "Expected 'x = W, y = H' line");

    if (result.width == 0 || result.height == 0)
        throw PlanesConverterException("Invalid dimensions",
                                        "width or height is zero");

    result.cells.resize(result.width * result.height, 0);

    size_t cx = 0;
    size_t cy = 0;
    size_t runCount = 0;

    for (char ch : body) {
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
            continue;

        if (ch == '!')
            break;

        if (std::isdigit(static_cast<unsigned char>(ch))) {
            runCount = runCount * 10 + static_cast<size_t>(ch - '0');
            continue;
        }

        size_t count = (runCount == 0) ? 1 : runCount;
        runCount = 0;

        if (ch == '$') {
            cy += count;
            cx = 0;
            continue;
        }

        uint8_t val = tagToValue(ch);

        for (size_t i = 0; i < count; ++i) {
            if (cy < result.height && cx < result.width)
                result.cells[cy * result.width + cx] = val;
            ++cx;
        }
    }

    return result;
}

void LifeRlePlanesConverter::stampOnPlanes(const ParsedRle& parsed,
                                            Planes& planes,
                                            size_t ox, size_t oy)
{
    uint8_t* dst = planes.data();
    size_t stride = planes.width();

    for (size_t y = 0; y < parsed.height; ++y) {
        size_t fy = oy + y;
        if (fy >= planes.height())
            break;
        for (size_t x = 0; x < parsed.width; ++x) {
            size_t fx = ox + x;
            if (fx >= planes.width())
                break;
            uint8_t val = parsed.cells[y * parsed.width + x];
            if (val != 0)
                dst[fy * stride + fx] = val;
        }
    }
}

// ── PlanesConverter interface ───────────────────────────

void LifeRlePlanesConverter::serialize(const Planes& planes,
                                        std::ostream& out) const
{
    size_t W = planes.width();
    size_t H = planes.height();
    auto view = planes.view(0);

    out << "#C Generated by ntcap\n";
    out << "x = " << W << ", y = " << H << "\n";

    size_t lastRow = 0;
    for (size_t y = 0; y < H; ++y) {
        for (size_t x = 0; x < W; ++x) {
            if (view[y * W + x] != 0) {
                lastRow = y;
                break;
            }
        }
    }

    size_t lineLen = 0;
    constexpr size_t kMaxLine = 70;

    auto emit = [&](const std::string& token) {
        if (lineLen + token.size() > kMaxLine && lineLen > 0) {
            out << '\n';
            lineLen = 0;
        }
        out << token;
        lineLen += token.size();
    };

    size_t pendingRows = 0;

    for (size_t y = 0; y <= lastRow; ++y) {
        size_t lastCol = 0;
        bool hasContent = false;
        for (size_t x = 0; x < W; ++x) {
            if (view[y * W + x] != 0) {
                lastCol = x;
                hasContent = true;
            }
        }

        if (!hasContent && y < lastRow) {
            ++pendingRows;
            continue;
        }

        if (pendingRows > 0) {
            size_t total = pendingRows + (hasContent ? 0 : 1);
            if (total > 1)
                emit(std::to_string(total) + "$");
            else
                emit("$");
            pendingRows = 0;
        }

        if (!hasContent) {
            if (y < lastRow)
                emit("$");
            continue;
        }

        size_t x = 0;
        while (x <= lastCol) {
            uint8_t val = view[y * W + x];
            char tag = valueToTag(val);
            size_t run = 1;
            while (x + run <= lastCol && view[y * W + x + run] == val)
                ++run;

            if (run > 1)
                emit(std::to_string(run) + std::string(1, tag));
            else
                emit(std::string(1, tag));

            x += run;
        }

        if (y < lastRow)
            emit("$");
    }

    emit("!");
    out << '\n';
}

Planes LifeRlePlanesConverter::deserialize(std::istream& in) const
{
    auto parsed = parseStream(in);

    Planes planes(parsed.width, parsed.height, 1);
    std::memcpy(planes.data(), parsed.cells.data(),
                parsed.width * parsed.height);
    return planes;
}

// ── Static convenience methods ──────────────────────────

Planes LifeRlePlanesConverter::fromString(const std::string& rle,
                                           size_t numPlanes,
                                           size_t margin)
{
    std::istringstream ss(rle);
    auto parsed = parseStream(ss);

    size_t fw = parsed.width + 2 * margin;
    size_t fh = parsed.height + 2 * margin;

    Planes planes(fw, fh, numPlanes);
    stampOnPlanes(parsed, planes, margin, margin);
    return planes;
}

Planes LifeRlePlanesConverter::fromString(const std::string& rle,
                                           size_t fieldWidth,
                                           size_t fieldHeight,
                                           size_t numPlanes)
{
    std::istringstream ss(rle);
    auto parsed = parseStream(ss);

    if (parsed.width > fieldWidth || parsed.height > fieldHeight)
        throw PlanesConverterException(
            "Pattern too large",
            "Pattern is " + std::to_string(parsed.width) + "x"
            + std::to_string(parsed.height) + " but field is "
            + std::to_string(fieldWidth) + "x"
            + std::to_string(fieldHeight));

    Planes planes(fieldWidth, fieldHeight, numPlanes);
    size_t ox = (fieldWidth - parsed.width) / 2;
    size_t oy = (fieldHeight - parsed.height) / 2;
    stampOnPlanes(parsed, planes, ox, oy);
    return planes;
}

} // namespace ntcap