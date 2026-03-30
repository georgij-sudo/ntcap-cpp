#include "VonNeumannN.h"

namespace ntcap {

// ── VonNeumannN ─────────────────────────────────────────────

VonNeumannN::VonNeumannN(uint8_t center,
                         uint8_t north,
                         uint8_t east,
                         uint8_t south,
                         uint8_t west)
    : m_center{center}
    , m_north{north}
    , m_east{east}
    , m_south{south}
    , m_west{west}
{}

uint8_t VonNeumannN::center() const noexcept { return m_center; }
uint8_t VonNeumannN::north()  const noexcept { return m_north; }
uint8_t VonNeumannN::east()   const noexcept { return m_east; }
uint8_t VonNeumannN::south()  const noexcept { return m_south; }
uint8_t VonNeumannN::west()   const noexcept { return m_west; }

uint8_t VonNeumannN::count() const noexcept {
    uint8_t result = 0;
    if (m_center != 0) ++result;
    if (m_north != 0)  ++result;
    if (m_east  != 0)  ++result;
    if (m_south != 0)  ++result;
    if (m_west  != 0)  ++result;
    return result;
}

// countIf реализован в заголовке (шаблон)

// ── VonNeumannNBuilder ──────────────────────────────────────

VonNeumannNBuilder& VonNeumannNBuilder::center(uint8_t value) noexcept {
    m_center = value;
    return *this;
}

VonNeumannNBuilder& VonNeumannNBuilder::north(uint8_t value) noexcept {
    m_north = value;
    return *this;
}

VonNeumannNBuilder& VonNeumannNBuilder::east(uint8_t value) noexcept {
    m_east = value;
    return *this;
}

VonNeumannNBuilder& VonNeumannNBuilder::south(uint8_t value) noexcept {
    m_south = value;
    return *this;
}

VonNeumannNBuilder& VonNeumannNBuilder::west(uint8_t value) noexcept {
    m_west = value;
    return *this;
}

VonNeumannN VonNeumannNBuilder::build() noexcept {
    return VonNeumannN{m_center, m_north, m_east, m_south, m_west};
}

} // namespace ntcap
