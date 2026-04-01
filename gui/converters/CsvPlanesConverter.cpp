// CsvPlanesConverter.cpp
#include "CsvPlanesConverter.h"
#include "PlanesCsvConverter.h"  // core.lib

namespace ntcap {

void CsvPlanesConverter::serialize(const Planes& planes, std::ostream& out) const {
    try {
        PlanesCsvConverter::serialize(planes, out);
    } catch (const PlanesCsvConverterException& e) {
        throw PlanesConverterException(e.message(), e.csv());
    }
}

Planes CsvPlanesConverter::deserialize(std::istream& in) const {
    try {
        return PlanesCsvConverter::deserialize(in);
    } catch (const PlanesCsvConverterException& e) {
        throw PlanesConverterException(e.message(), e.csv());
    }
}

} // namespace ntcap