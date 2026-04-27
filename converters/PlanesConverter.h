#pragma once

#include "Planes.h"

#include <iosfwd>
#include <exception>
#include <string>

namespace ntcap {

class PlanesConverterException : public std::exception {
public:
    PlanesConverterException(std::string message, std::string detail);

    const char* what() const noexcept override;
    const std::string& message() const noexcept;
    const std::string& detail() const noexcept;

private:
    std::string m_message;
    std::string m_detail;
    std::string m_what;
};

class PlanesConverter {
public:
    virtual ~PlanesConverter() = default;

    virtual void   serialize(const Planes& planes, std::ostream& out) const = 0;
    virtual Planes deserialize(std::istream& in) const = 0;
};

} // namespace ntcap