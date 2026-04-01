#include "PlanesRleConverter.h"
#include <sstream>
#include <cstring>
#include <iterator>

namespace ntcap {

// ───────────── PlanesRleConverterException ─────────────

PlanesRleConverterException::PlanesRleConverterException(std::string message, size_t offset)
    : m_message(std::move(message))
    , m_offset(offset)
{
    m_what = m_message + " (byte offset: " + std::to_string(m_offset) + ")";
}

const char* PlanesRleConverterException::what() const noexcept {
    return m_what.c_str();
}

const std::string& PlanesRleConverterException::message() const noexcept {
    return m_message;
}

size_t PlanesRleConverterException::offset() const noexcept {
    return m_offset;
}

// ───────────── Little-endian helpers ─────────────

void PlanesRleConverter::writeLE64(std::vector<uint8_t>& buf, uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        buf.push_back(static_cast<uint8_t>(v & 0xFF));
        v >>= 8;
    }
}

void PlanesRleConverter::writeLE32(std::vector<uint8_t>& buf, uint32_t v) {
    for (int i = 0; i < 4; ++i) {
        buf.push_back(static_cast<uint8_t>(v & 0xFF));
        v >>= 8;
    }
}

uint64_t PlanesRleConverter::readLE64(const uint8_t* p) {
    uint64_t v = 0;
    for (int i = 7; i >= 0; --i) {
        v = (v << 8) | p[i];
    }
    return v;
}

uint32_t PlanesRleConverter::readLE32(const uint8_t* p) {
    uint32_t v = 0;
    for (int i = 3; i >= 0; --i) {
        v = (v << 8) | p[i];
    }
    return v;
}

// ───────────── RLE encode / decode ─────────────

std::vector<PlanesRleConverter::Run>
PlanesRleConverter::encode(std::span<const uint8_t> plane) {
    std::vector<Run> runs;
    if (plane.empty()) return runs;

    uint8_t  curVal   = plane[0];
    uint32_t curCount = 1;

    for (size_t i = 1; i < plane.size(); ++i) {
        if (plane[i] == curVal && curCount < UINT32_MAX) {
            ++curCount;
        } else {
            runs.push_back({curVal, curCount});
            curVal   = plane[i];
            curCount = 1;
        }
    }
    runs.push_back({curVal, curCount});
    return runs;
}

void PlanesRleConverter::decode(const std::vector<Run>& runs,
                                 uint8_t* dst, size_t expectedSize) {
    size_t pos = 0;
    for (const auto& r : runs) {
        if (pos + r.count > expectedSize) {
            throw PlanesRleConverterException(
                "RLE decode overflow: run exceeds plane size", 0);
        }
        std::memset(dst + pos, r.value, r.count);
        pos += r.count;
    }
    if (pos != expectedSize) {
        throw PlanesRleConverterException(
            "RLE decode underflow: decoded " + std::to_string(pos)
            + " cells, expected " + std::to_string(expectedSize), 0);
    }
}

// ───────────── serialize ─────────────

std::vector<uint8_t> PlanesRleConverter::serialize(const Planes& planes) {
    std::vector<uint8_t> buf;
    buf.reserve(HEADER_SIZE + planes.numPlanes() * planes.size() * 5);

    // magic
    buf.insert(buf.end(), MAGIC, MAGIC + 4);
    // version
    buf.push_back(VERSION);
    // dimensions
    writeLE64(buf, static_cast<uint64_t>(planes.width()));
    writeLE64(buf, static_cast<uint64_t>(planes.height()));
    writeLE64(buf, static_cast<uint64_t>(planes.numPlanes()));

    for (size_t p = 0; p < planes.numPlanes(); ++p) {
        auto planeView = planes.view(p);
        auto runs = encode(planeView);

        if (runs.size() > UINT32_MAX) {
            throw PlanesRleConverterException(
                "Too many RLE runs for plane " + std::to_string(p), buf.size());
        }

        writeLE32(buf, static_cast<uint32_t>(runs.size()));
        for (const auto& r : runs) {
            buf.push_back(r.value);
            writeLE32(buf, r.count);
        }
    }
    return buf;
}

void PlanesRleConverter::serialize(const Planes& planes, std::ostream& out) {
    auto buf = serialize(planes);
    out.write(reinterpret_cast<const char*>(buf.data()),
              static_cast<std::streamsize>(buf.size()));
    if (!out) {
        throw PlanesRleConverterException("Failed to write RLE to stream", 0);
    }
}

// ───────────── deserialize ─────────────

Planes PlanesRleConverter::deserialize(const uint8_t* data, size_t size) {
    size_t off = 0;

    // ── magic ──
    if (size < HEADER_SIZE) {
        throw PlanesRleConverterException("Incomplete header", off);
    }
    if (std::memcmp(data + off, MAGIC, 4) != 0) {
        throw PlanesRleConverterException("Invalid magic bytes", off);
    }
    off += 4;

    // ── version ──
    uint8_t ver = data[off++];
    if (ver != VERSION) {
        throw PlanesRleConverterException(
            "Unsupported RLE version: " + std::to_string(ver), off - 1);
    }

    // ── dimensions ──
    uint64_t width     = readLE64(data + off); off += 8;
    uint64_t height    = readLE64(data + off); off += 8;
    uint64_t numPlanes = readLE64(data + off); off += 8;

    if (width == 0 || height == 0 || numPlanes == 0) {
        throw PlanesRleConverterException(
            "Zero dimension: width=" + std::to_string(width)
            + " height=" + std::to_string(height)
            + " numPlanes=" + std::to_string(numPlanes), off);
    }

    const size_t planeSize = static_cast<size_t>(width) * static_cast<size_t>(height);

    Planes planes(static_cast<size_t>(width),
                  static_cast<size_t>(height),
                  static_cast<size_t>(numPlanes));

    for (size_t p = 0; p < numPlanes; ++p) {
        // numRuns
        if (off + 4 > size) {
            throw PlanesRleConverterException(
                "Unexpected end of data reading numRuns for plane "
                + std::to_string(p), off);
        }
        uint32_t numRuns = readLE32(data + off); off += 4;

        std::vector<Run> runs(numRuns);
        for (uint32_t r = 0; r < numRuns; ++r) {
            if (off + 5 > size) {
                throw PlanesRleConverterException(
                    "Unexpected end of data reading run " + std::to_string(r)
                    + " of plane " + std::to_string(p), off);
            }
            runs[r].value = data[off++];
            runs[r].count = readLE32(data + off); off += 4;
        }

        uint8_t* dst = planes.data() + p * planeSize;
        decode(runs, dst, planeSize);
    }

    return planes;
}

Planes PlanesRleConverter::deserialize(const std::vector<uint8_t>& rle) {
    return deserialize(rle.data(), rle.size());
}

Planes PlanesRleConverter::deserialize(std::istream& in) {
    std::vector<uint8_t> buf{std::istreambuf_iterator<char>{in},
                             std::istreambuf_iterator<char>{}};
    if (!in && !in.eof()) {
        throw PlanesRleConverterException("Failed to read RLE from stream", 0);
    }
    return deserialize(buf.data(), buf.size());
}

} // namespace ntcap