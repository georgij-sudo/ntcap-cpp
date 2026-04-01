#include "PlanesCsvConverter.h"

#include <sstream>
#include <string>

namespace ntcap {

// ---------------------------------------------------------------------------
// PlanesCsvConverterException
// ---------------------------------------------------------------------------

PlanesCsvConverterException::PlanesCsvConverterException(std::string message, std::string csv)
    : m_message(std::move(message))
    , m_csv(std::move(csv))
    , m_what(m_message + "\n[csv]: \"" + m_csv + "\"")
{
}

const char* PlanesCsvConverterException::what() const noexcept
{
    return m_what.c_str();
}

const std::string& PlanesCsvConverterException::message() const noexcept
{
    return m_message;
}

const std::string& PlanesCsvConverterException::csv() const noexcept
{
    return m_csv;
}

// ---------------------------------------------------------------------------
// Вспомогательные функции
// ---------------------------------------------------------------------------
namespace {

std::string trimToken(const std::string& token)
{
    const size_t start = token.find_first_not_of(" \t\r");
    const size_t end   = token.find_last_not_of(" \t\r");
    if (start == std::string::npos) {
        return {};
    }
    return token.substr(start, end - start + 1);
}

/**
 * Парсит строку CSV в вектор unsigned long значений.
 * НЕ проверяет диапазон — это ответственность вызывающего кода.
 */
std::vector<unsigned long> parseCsvLine(const std::string& line, const std::string& fullCsv)
{
    std::vector<unsigned long> values;
    std::istringstream iss(line);
    std::string token;

    while (std::getline(iss, token, ',')) {
        std::string trimmed = trimToken(token);
        if (trimmed.empty()) {
            throw PlanesCsvConverterException(
                "empty token in CSV line: \"" + line + "\"", fullCsv);
        }

        unsigned long val{};
        try {
            size_t pos{};
            val = std::stoul(trimmed, &pos);
            if (pos != trimmed.size()) {
                throw PlanesCsvConverterException(
                    "invalid token \"" + trimmed + "\" in line: \"" + line + "\"", fullCsv);
            }
        } catch (const PlanesCsvConverterException&) {
            throw;
        } catch (const std::exception& e) {
            throw PlanesCsvConverterException(
                "failed to parse token \"" + trimmed + "\": " + e.what(), fullCsv);
        }

        values.push_back(val);
    }
    return values;
}

/**
 * Парсит строку CSV и проверяет, что каждое значение ≤ 255.
 */
std::vector<uint8_t> parseCsvCellLine(const std::string& line, const std::string& fullCsv)
{
    auto raw = parseCsvLine(line, fullCsv);
    std::vector<uint8_t> cells;
    cells.reserve(raw.size());
    for (auto val : raw) {
        if (val > 255) {
            throw PlanesCsvConverterException(
                "cell value " + std::to_string(val) + " out of uint8_t range [0..255]", fullCsv);
        }
        cells.push_back(static_cast<uint8_t>(val));
    }
    return cells;
}

bool nextNonEmptyLine(std::istream& in, std::string& line)
{
    while (std::getline(in, line)) {
        if (line.find_first_not_of(" \t\r") != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool hasMoreNonEmptyData(std::istream& in)
{
    std::string line;
    while (std::getline(in, line)) {
        if (line.find_first_not_of(" \t\r") != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// PlanesCsvConverter::serialize
// ---------------------------------------------------------------------------

void PlanesCsvConverter::serialize(const Planes& planes, std::ostream& out)
{
    const size_t w         = planes.width();
    const size_t h         = planes.height();
    const size_t planeSize = planes.size();
    const size_t totalSize = planes.view().size();

    if (planeSize == 0) {
        throw PlanesCsvConverterException("cannot serialize Planes with zero size", "");
    }

    const size_t numPlanes = totalSize / planeSize;

    out << w << ',' << h << ',' << numPlanes << '\n';
    if (out.fail()) {
        throw PlanesCsvConverterException("stream write failed while writing header", "");
    }

    for (size_t p = 0; p < numPlanes; ++p) {
        if (p > 0) {
            out << '\n';
        }

        auto plane = planes.view(p);

        for (size_t y = 0; y < h; ++y) {
            for (size_t x = 0; x < w; ++x) {
                if (x > 0) {
                    out << ',';
                }
                out << static_cast<unsigned>(plane[y * w + x]);
            }
            out << '\n';

            if (out.fail()) {
                std::string partial;
                if (auto* oss = dynamic_cast<std::ostringstream*>(&out)) {
                    partial = oss->str();
                }
                throw PlanesCsvConverterException(
                    "stream write failed at plane " + std::to_string(p)
                    + ", row " + std::to_string(y),
                    partial);
            }
        }
    }
}

std::string PlanesCsvConverter::serialize(const Planes& planes)
{
    std::ostringstream oss;
    serialize(planes, oss);
    return oss.str();
}

// ---------------------------------------------------------------------------
// PlanesCsvConverter::deserialize
// ---------------------------------------------------------------------------

Planes PlanesCsvConverter::deserialize(std::istream& in)
{
    std::string consumed;

    auto readLine = [&](const std::string& context) -> std::string {
        std::string line;
        if (!nextNonEmptyLine(in, line)) {
            throw PlanesCsvConverterException(
                "unexpected end of input: " + context, consumed);
        }
        consumed += line + '\n';
        return line;
    };

    // --- Заголовок ----------------------------------------------------------
    std::string headerLine = readLine("expected header (width,height,numPlanes)");
    auto headerTokens = parseCsvLine(headerLine, consumed);  // без проверки диапазона

    if (headerTokens.size() != 3) {
        throw PlanesCsvConverterException(
            "header must contain exactly 3 values (width,height,numPlanes), got "
            + std::to_string(headerTokens.size()),
            consumed);
    }

    const size_t width     = static_cast<size_t>(headerTokens[0]);
    const size_t height    = static_cast<size_t>(headerTokens[1]);
    const size_t numPlanes = static_cast<size_t>(headerTokens[2]);

    if (width == 0) {
        throw PlanesCsvConverterException("width must be > 0", consumed);
    }
    if (height == 0) {
        throw PlanesCsvConverterException("height must be > 0", consumed);
    }
    if (numPlanes == 0) {
        throw PlanesCsvConverterException("numPlanes must be > 0", consumed);
    }

    // --- Данные -------------------------------------------------------------
    Planes planes(width, height, numPlanes);
    uint8_t* dst = planes.data();

    for (size_t p = 0; p < numPlanes; ++p) {
        for (size_t y = 0; y < height; ++y) {
            std::string line = readLine(
                "expected data at plane " + std::to_string(p)
                + ", row " + std::to_string(y));

            auto cells = parseCsvCellLine(line, consumed);  // с проверкой [0..255]

            if (cells.size() != width) {
                throw PlanesCsvConverterException(
                    "expected " + std::to_string(width) + " values in row, got "
                    + std::to_string(cells.size())
                    + " (plane " + std::to_string(p) + ", row " + std::to_string(y) + ")",
                    consumed);
            }

            const size_t offset = p * (width * height) + y * width;
            for (size_t x = 0; x < width; ++x) {
                dst[offset + x] = cells[x];
            }
        }
    }

    // --- Проверка на лишние данные ------------------------------------------
    if (hasMoreNonEmptyData(in)) {
        throw PlanesCsvConverterException(
            "excess data after expected " + std::to_string(numPlanes)
            + " plane(s) of " + std::to_string(width) + "x" + std::to_string(height),
            consumed);
    }

    return planes;
}

Planes PlanesCsvConverter::deserialize(const std::string& csv)
{
    std::istringstream iss(csv);
    return deserialize(iss);
}

} // namespace ntcap