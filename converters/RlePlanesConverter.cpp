#include "RlePlanesConverter.h"

#include "PlanesRleConverter.h"

namespace ntcap {

void RlePlanesConverter::serialize(const Planes& planes, std::ostream& out) const {
    try {
        PlanesRleConverter::serialize(planes, out);
    } catch (const PlanesRleConverterException& e) {
        throw PlanesConverterException(e.message(), std::to_string(e.offset()));
    }
}

Planes RlePlanesConverter::deserialize(std::istream& in) const {
    try {
        return PlanesRleConverter::deserialize(in);
    } catch (const PlanesRleConverterException& e) {
        throw PlanesConverterException(e.message(), std::to_string(e.offset()));
    }
}

} // namespace ntcap