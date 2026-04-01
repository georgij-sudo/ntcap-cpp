#include "MooreN.h"

namespace ntcap {

// ── MooreN ──────────────────────────────────────────────────

MooreN::MooreN(uint8_t center,
               uint8_t north,
               uint8_t northEast,
               uint8_t east,
               uint8_t southEast,
               uint8_t south,
               uint8_t southWest,
               uint8_t west,
               uint8_t northWest)
    : m_center{center}
    , m_north{north}
    , m_northEast{northEast}
    , m_east{east}
    , m_southEast{southEast}
    , m_south{south}
    , m_southWest{southWest}
    , m_west{west}
    , m_northWest{northWest}
{}

uint8_t MooreN::center()    const noexcept { return m_center; }
uint8_t MooreN::north()     const noexcept { return m_north; }
uint8_t MooreN::northEast() const noexcept { return m_northEast; }
uint8_t MooreN::east()      const noexcept { return m_east; }
uint8_t MooreN::southEast() const noexcept { return m_southEast; }
uint8_t MooreN::south()     const noexcept { return m_south; }
uint8_t MooreN::southWest() const noexcept { return m_southWest; }
uint8_t MooreN::west()      const noexcept { return m_west; }
uint8_t MooreN::northWest() const noexcept { return m_northWest; }

uint8_t MooreN::count() const noexcept {
    uint8_t result = 0;
    if (m_center    != 0) ++result;
    if (m_north     != 0) ++result;
    if (m_northEast != 0) ++result;
    if (m_east      != 0) ++result;
    if (m_southEast != 0) ++result;
    if (m_south     != 0) ++result;
    if (m_southWest != 0) ++result;
    if (m_west      != 0) ++result;
    if (m_northWest != 0) ++result;
    return result;
}

// countIf реализован в заголовке (шаблон)

// ── MooreNBuilder ───────────────────────────────────────────

MooreNBuilder& MooreNBuilder::center(uint8_t value) noexcept {
    m_center = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::north(uint8_t value) noexcept {
    m_north = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::northEast(uint8_t value) noexcept {
    m_northEast = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::east(uint8_t value) noexcept {
    m_east = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::southEast(uint8_t value) noexcept {
    m_southEast = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::south(uint8_t value) noexcept {
    m_south = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::southWest(uint8_t value) noexcept {
    m_southWest = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::west(uint8_t value) noexcept {
    m_west = value;
    return *this;
}

MooreNBuilder& MooreNBuilder::northWest(uint8_t value) noexcept {
    m_northWest = value;
    return *this;
}

MooreN MooreNBuilder::build() noexcept {
    return MooreN{m_center, m_north, m_northEast, m_east,
                  m_southEast, m_south, m_southWest, m_west,
                  m_northWest};
}

} // namespace ntcap