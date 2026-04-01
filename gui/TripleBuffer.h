#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <type_traits>

namespace ntcap {

template <typename T>
class TripleBuffer {
public:
    /// Инициализирует все три слота копией proto
    explicit TripleBuffer(const T& proto)
        : m_buf{proto, proto, proto}
    {}

    /// Инициализирует все три слота через фабрику
    template <typename Factory,
              std::enable_if_t<
                  std::is_invocable_r_v<T, Factory&>
                  && !std::is_same_v<std::decay_t<Factory>, T>
                  && !std::is_same_v<std::decay_t<Factory>, TripleBuffer>,
              int> = 0>
    explicit TripleBuffer(Factory&& factory)
        : m_buf{factory(), factory(), factory()}
    {}

    // ── Writer thread ───────────────────────────────

    T& writeBuffer() noexcept {
        return m_buf[m_write];
    }

    bool publish() noexcept {
        uint32_t old = m_shared.load(std::memory_order_acquire);
        uint32_t next;
        do {
            uint8_t readyIdx = old & 0x3;
            next = m_write | kNewDataFlag;
            m_write = readyIdx;
        } while (!m_shared.compare_exchange_weak(
                     old, next,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire));
        return (old & kNewDataFlag) != 0;
    }

    // ── Reader thread ───────────────────────────────

    bool hasNew() const noexcept {
        return (m_shared.load(std::memory_order_acquire) & kNewDataFlag) != 0;
    }

    bool consume() noexcept {
        uint32_t old = m_shared.load(std::memory_order_acquire);
        if (!(old & kNewDataFlag))
            return false;
        uint32_t next;
        do {
            if (!(old & kNewDataFlag))
                return false;
            uint8_t readyIdx = old & 0x3;
            next = m_read;
            m_read = readyIdx;
        } while (!m_shared.compare_exchange_weak(
                     old, next,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire));
        return true;
    }

    const T& readBuffer() const noexcept {
        return m_buf[m_read];
    }

private:
    static constexpr uint32_t kNewDataFlag = 0x4;

    std::array<T, 3> m_buf;
    uint8_t m_write = 0;
    uint8_t m_read  = 2;
    std::atomic<uint32_t> m_shared{1};
};

} // namespace ntcap