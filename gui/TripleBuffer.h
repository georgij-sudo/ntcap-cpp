#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <type_traits>
#include <utility>

namespace ntcap {

template <typename T>
class TripleBuffer {
public:
    explicit TripleBuffer(const T& proto)
        : m_buf{proto, proto, proto}
    {}

    /// Конструктор из фабрики — безопаснее, чем копирование
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
        const uint8_t w = m_write;
        uint32_t old = m_shared.load(std::memory_order_acquire);
        uint32_t next;
        do {
            next = w | kNewDataFlag;
        } while (!m_shared.compare_exchange_weak(
                     old, next,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire));
        m_write = old & kIdxMask;
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

        const uint8_t r = m_read;
        uint32_t next;
        do {
            if (!(old & kNewDataFlag))
                return false;
            next = r;
        } while (!m_shared.compare_exchange_weak(
                     old, next,
                     std::memory_order_acq_rel,
                     std::memory_order_acquire));
        m_read = old & kIdxMask;
        return true;
    }

    const T& readBuffer() const noexcept {
        return m_buf[m_read];
    }

private:
    static constexpr uint32_t kNewDataFlag = 0x4;
    static constexpr uint32_t kIdxMask     = 0x3;

    std::array<T, 3> m_buf;
    uint8_t m_write = 0;
    uint8_t m_read  = 2;
    std::atomic<uint32_t> m_shared{1};
};

} // namespace ntcap