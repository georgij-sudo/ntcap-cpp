#pragma once

#include "Topology.h"

namespace ntcap {

class TorusTopology final : public Topology {
public:
    TorusTopology(size_t width, size_t height);

public:
    UCoord2D normalize(const Coord2D&) override;

private:
    size_t m_width;
    size_t m_height;
};

} // namespace ntcap

