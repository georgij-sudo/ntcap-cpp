#pragma once
#include <cstdint>
#include <concepts>

namespace ntcap {

class MooreN final {
public:
    uint8_t center()    const noexcept;
    uint8_t north()     const noexcept;
    uint8_t northEast() const noexcept;
    uint8_t east()      const noexcept;
    uint8_t southEast() const noexcept;
    uint8_t south()     const noexcept;
    uint8_t southWest() const noexcept;
    uint8_t west()      const noexcept;
    uint8_t northWest() const noexcept;

    uint8_t count() const noexcept;

    template<typename Pred>
        requires std::predicate<Pred, uint8_t>
    uint8_t countIf(Pred pred) const noexcept;

private:
    friend class MooreNBuilder;

    MooreN(uint8_t center,
           uint8_t north,
           uint8_t northEast,
           uint8_t east,
           uint8_t southEast,
           uint8_t south,
           uint8_t southWest,
           uint8_t west,
           uint8_t northWest);

    const uint8_t m_center;
    const uint8_t m_north;
    const uint8_t m_northEast;
    const uint8_t m_east;
    const uint8_t m_southEast;
    const uint8_t m_south;
    const uint8_t m_southWest;
    const uint8_t m_west;
    const uint8_t m_northWest;
};

class MooreNBuilder final {
public:
    MooreNBuilder& center   (uint8_t value) noexcept;
    MooreNBuilder& north    (uint8_t value) noexcept;
    MooreNBuilder& northEast(uint8_t value) noexcept;
    MooreNBuilder& east     (uint8_t value) noexcept;
    MooreNBuilder& southEast(uint8_t value) noexcept;
    MooreNBuilder& south    (uint8_t value) noexcept;
    MooreNBuilder& southWest(uint8_t value) noexcept;
    MooreNBuilder& west     (uint8_t value) noexcept;
    MooreNBuilder& northWest(uint8_t value) noexcept;

    MooreN build() noexcept;

private:
    uint8_t m_center    = 0;
    uint8_t m_north     = 0;
    uint8_t m_northEast = 0;
    uint8_t m_east      = 0;
    uint8_t m_southEast = 0;
    uint8_t m_south     = 0;
    uint8_t m_southWest = 0;
    uint8_t m_west      = 0;
    uint8_t m_northWest = 0;
};

template<typename Pred>
    requires std::predicate<Pred, uint8_t>
uint8_t MooreN::countIf(Pred pred) const noexcept {
    uint8_t result = 0;
    if (pred(m_center))    ++result;
    if (pred(m_north))     ++result;
    if (pred(m_northEast)) ++result;
    if (pred(m_east))      ++result;
    if (pred(m_southEast)) ++result;
    if (pred(m_south))     ++result;
    if (pred(m_southWest)) ++result;
    if (pred(m_west))      ++result;
    if (pred(m_northWest)) ++result;
    return result;
}

} // namespace ntcap