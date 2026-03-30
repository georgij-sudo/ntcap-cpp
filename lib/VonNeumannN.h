#pragma once

#include <cstdint>
#include <concepts>

namespace ntcap {

class VonNeumannN final {
public:
    uint8_t center() const noexcept;

    uint8_t north() const noexcept;
    uint8_t east() const noexcept;
    uint8_t south() const noexcept;
    uint8_t west() const noexcept;

    uint8_t count() const noexcept;

    template<typename Pred>
        requires std::predicate<Pred, uint8_t>
    uint8_t countIf(Pred pred) const noexcept;

private:
    friend class VonNeumannNBuilder;

    VonNeumannN(uint8_t center,
                uint8_t north,
                uint8_t east,
                uint8_t south,
                uint8_t west);

    const uint8_t m_center;
    const uint8_t m_north;
    const uint8_t m_east;
    const uint8_t m_south;
    const uint8_t m_west;
};

class VonNeumannNBuilder final {
public:
    VonNeumannNBuilder& center(uint8_t value) noexcept;
    VonNeumannNBuilder& north (uint8_t value) noexcept;
    VonNeumannNBuilder& east  (uint8_t value) noexcept;
    VonNeumannNBuilder& south (uint8_t value) noexcept;
    VonNeumannNBuilder& west  (uint8_t value) noexcept;

    VonNeumannN build() noexcept;
private:
    uint8_t m_center = 0;
    uint8_t m_north  = 0;
    uint8_t m_east   = 0;
    uint8_t m_south  = 0;
    uint8_t m_west   = 0;
};

template<typename Pred>
    requires std::predicate<Pred, uint8_t>
uint8_t VonNeumannN::countIf(Pred pred) const noexcept {
    uint8_t result = 0;
    if (pred(m_center)) ++result;
    if (pred(m_north))  ++result;
    if (pred(m_east))   ++result;
    if (pred(m_south))  ++result;
    if (pred(m_west))   ++result;
    return result;
}

} // namespace ntcap
