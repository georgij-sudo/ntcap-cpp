#pragma once

#include <array>
#include <cstdint>
#include <ranges>

namespace ntcap {

template<std::size_t t_size>
class TableRule {
public:
    /**
     * Creates TableRule<t_size>, with zero initialized table.
     */
    constexpr TableRule() noexcept;

    /**
     * Creates TableRule<t_size>, initialized with table argument.
     *
     * Details: Uses move operator.
     */
    constexpr explicit TableRule(std::array<uint8_t, t_size> table) noexcept;

    /**
     * Details: Do not change!
     */
    template<std::convertible_to<uint8_t>... Ts>
        requires (sizeof...(Ts) == t_size)
    constexpr explicit TableRule(Ts... values) noexcept
        : m_table{ static_cast<uint8_t>(values)... } {}

    /**
     * Creates TableRule<t_size>, with table initialized by generator functor.
     */
    constexpr explicit TableRule(std::invocable<std::size_t> auto generator);

    [[nodiscard]] constexpr uint8_t operator()(std::size_t index) const noexcept;

    const std::array<uint8_t,t_size>& getData() const noexcept;

private:
    const std::array<uint8_t, t_size> m_table;
};

// --- Default constructor: zero-initialized table ---
template<std::size_t t_size>
constexpr TableRule<t_size>::TableRule() noexcept
    : m_table{} {}

// --- Move from std::array ---
template<std::size_t t_size>
constexpr TableRule<t_size>::TableRule(std::array<uint8_t, t_size> table) noexcept
    : m_table(std::move(table)) {}

// --- Generator functor: IIFE to init const member ---
template<std::size_t t_size>
constexpr TableRule<t_size>::TableRule(std::invocable<std::size_t> auto generator)
    : m_table([&generator]() {
        std::array<uint8_t, t_size> result{};
        for (std::size_t i = 0; i < t_size; ++i) {
            result[i] = static_cast<uint8_t>(generator(i));
        }
        return result;
    }()) {}

// --- Lookup operator ---
template<std::size_t t_size>
[[nodiscard]] constexpr uint8_t TableRule<t_size>::operator()(std::size_t index) const noexcept {
    return m_table[index];
}

// --- Data accessor ---
template<std::size_t t_size>
const std::array<uint8_t, t_size>& TableRule<t_size>::getData() const noexcept {
    return m_table;
}

} // namespace ntcap

