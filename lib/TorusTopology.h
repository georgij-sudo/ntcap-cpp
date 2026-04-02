#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace ntcap {

class TorusTopology final {
public:
    TorusTopology(size_t width, size_t height);

    // ── быстрые не-виртуальные обёртки для ±1 от границы ──
    //    Для координат в диапазоне [-1 .. dim] — без деления.
    size_t wrapX(ptrdiff_t x) const noexcept
    {
        if (x < 0)                              return m_width  - 1;
        if (x >= static_cast<ptrdiff_t>(m_width))  return 0;
        return static_cast<size_t>(x);
    }

    size_t wrapY(ptrdiff_t y) const noexcept
    {
        if (y < 0)                               return m_height - 1;
        if (y >= static_cast<ptrdiff_t>(m_height))  return 0;
        return static_cast<size_t>(y);
    }

    size_t width()  const noexcept { return m_width;  }
    size_t height() const noexcept { return m_height; }

private:
    size_t m_width;
    size_t m_height;
};

} // namespace ntcap