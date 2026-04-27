#pragma once

#include "Planes.h"
#include <exception>
#include <string>
#include <iosfwd>

namespace ntcap {

/**
 * Исключение, выбрасываемое при ошибках сериализации/десериализации Planes в CSV.
 * Хранит сообщение об ошибке и CSV-строку (исходную или частично сформированную),
 * на которой произошла ошибка.
 */
class PlanesCsvConverterException : public std::exception {
public:
    /**
     * @param message  описание ошибки.
     * @param csv      исходный (при десериализации) или недописанный (при сериализации) CSV.
     */
    PlanesCsvConverterException(std::string message, std::string csv);

    const char* what() const noexcept override;

    /**
     * Возвращает описание ошибки (без CSV).
     */
    const std::string& message() const noexcept;

    /**
     * Возвращает CSV-строку, связанную с ошибкой.
     */
    const std::string& csv() const noexcept;

private:
    std::string m_message;
    std::string m_csv;
    std::string m_what;
};

/**
 * Сериализует/десериализует Planes в/из CSV формата.
 *
 * Формат CSV:
 *   Строка 1: width,height,numPlanes
 *   Далее для каждого plane (numPlanes штук) идут height строк,
 *   каждая из которых содержит width значений через запятую.
 *   Плоскости разделены пустой строкой.
 *
 * @throws PlanesCsvConverterException при любых ошибках сериализации/десериализации.
 */
class PlanesCsvConverter {
public:
    PlanesCsvConverter() = delete;

    static void serialize(const Planes& planes, std::ostream& out);
    static std::string serialize(const Planes& planes);

    static Planes deserialize(std::istream& in);
    static Planes deserialize(const std::string& csv);
};

} // namespace ntcap