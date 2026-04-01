#include "PlanesConverter.h"

namespace ntcap {

PlanesConverterException::PlanesConverterException(std::string message, std::string detail)
    : m_message(std::move(message))
    , m_detail(std::move(detail))
    , m_what(m_message + " [" + m_detail + "]")
{}

const char* PlanesConverterException::what() const noexcept {
    return m_what.c_str();
}

const std::string& PlanesConverterException::message() const noexcept {
    return m_message;
}

const std::string& PlanesConverterException::detail() const noexcept {
    return m_detail;
}

} // namespace ntcap