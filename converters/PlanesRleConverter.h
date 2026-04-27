#pragma once
#include "Planes.h"
#include <exception>
#include <string>
#include <vector>
#include <cstdint>
#include <iosfwd>

namespace ntcap {

/**
 * Исключение, выбрасываемое при ошибках сериализации/десериализации Planes в RLE.
 * Хранит сообщение об ошибке и позицию в потоке байт, на которой произошла ошибка.
 */
class PlanesRleConverterException : public std::exception {
public:
    /**
     * @param message  описание ошибки.
     * @param offset   смещение в байтовом потоке, на котором произошла ошибка.
     */
    PlanesRleConverterException(std::string message, size_t offset);
    const char* what() const noexcept override;

    /**
     * Возвращает описание ошибки.
     */
    const std::string& message() const noexcept;

    /**
     * Возвращает смещение в байтовом потоке, на котором возникла ошибка.
     */
    size_t offset() const noexcept;

private:
    std::string m_message;
    size_t      m_offset;
    std::string m_what;
};

/**
 * Сериализует/десериализует Planes в/из бинарного RLE формата.
 *
 * Бинарный формат (все многобайтовые поля — little-endian):
 *
 *   ┌─────────────────────────────────────────────┐
 *   │  magic        : 4 байта  "PRLE"             │
 *   │  version      : uint8_t  (== 1)             │
 *   │  width        : uint64_t                     │
 *   │  height       : uint64_t                     │
 *   │  numPlanes    : uint64_t                     │
 *   ├─────────────────────────────────────────────┤
 *   │  Для каждой плоскости (numPlanes штук):      │
 *   │    numRuns     : uint32_t                    │
 *   │    runs[numRuns]:                            │
 *   │      value     : uint8_t                     │
 *   │      count     : uint32_t                    │
 *   └─────────────────────────────────────────────┘
 *
 *   Сумма count по всем runs одной плоскости == width * height.
 *
 * @throws PlanesRleConverterException при любых ошибках сериализации/десериализации.
 */
class PlanesRleConverter {
public:
    PlanesRleConverter() = delete;

    static void               serialize(const Planes& planes, std::ostream& out);
    static std::vector<uint8_t> serialize(const Planes& planes);

    static Planes deserialize(std::istream& in);
    static Planes deserialize(const std::vector<uint8_t>& rle);
    static Planes deserialize(const uint8_t* data, size_t size);

private:
    static constexpr char     MAGIC[4]       = {'P', 'R', 'L', 'E'};
    static constexpr uint8_t  VERSION        = 1;
    static constexpr size_t   HEADER_SIZE    = 4 + 1 + 8 + 8 + 8; // 29 байт

    struct Run {
        uint8_t  value;
        uint32_t count;
    };

    static std::vector<Run> encode(std::span<const uint8_t> plane);
    static void             decode(const std::vector<Run>& runs, uint8_t* dst, size_t expectedSize);

    // helpers для little-endian чтения/записи
    static void     writeLE64(std::vector<uint8_t>& buf, uint64_t v);
    static void     writeLE32(std::vector<uint8_t>& buf, uint32_t v);
    static uint64_t readLE64(const uint8_t* p);
    static uint32_t readLE32(const uint8_t* p);
};

} // namespace ntcap