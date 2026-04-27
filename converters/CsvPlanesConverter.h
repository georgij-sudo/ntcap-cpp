#pragma once

#include "PlanesConverter.h"

namespace ntcap {

class CsvPlanesConverter final : public PlanesConverter {
public:
    void   serialize(const Planes& planes, std::ostream& out) const override;
    Planes deserialize(std::istream& in) const override;
};

} // namespace ntcap