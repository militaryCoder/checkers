#pragma once

#include "gfx/gl/types.hpp"

namespace primitives {
    auto circle(float r, size_t vertCount = 8) -> std::vector<gl::Position2Df>;
    auto rect(float w, float h) -> std::array<gl::Position2Df, 4>;
} // namespace primitives
