#pragma once

#include "Coord2D.h"
#include "UCoord2D.h"

namespace ntcap {

class Topology {
public:
    virtual ~Topology() = default;

    virtual UCoord2D normalize(const Coord2D&) = 0;
};

} // namespace ntcap

