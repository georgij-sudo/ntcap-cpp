#pragma once

namespace ntcap {

// Inline torusal coordinate wrapping
[[nodiscard]] inline size_t torusWrapX(ptrdiff_t x) const noexcept {
    const ptrdiff_t W = static_cast<ptrdiff_t>(current().width());
    return ((x % W) + W) % W;
}

[[nodiscard]] inline size_t torusWrapY(ptrdiff_t y) const noexcept {
    const ptrdiff_t H = static_cast<ptrdiff_t>(current().height());
    return ((y % H) + H) % H;
}

} // ntcap
