#include <array>
#include <cmath>
#include <vector>

#include "gfx/gl/types.hpp"

namespace primitives {
    auto circle(float r, size_t vertCount = 8) -> std::vector<gl::Position2Df> {
        static constexpr float PI = 3.1415926f;
        std::vector<gl::Position2Df> vertices;
        vertices.reserve(vertCount + 1); // Adding one position entry for central point
        vertices[0] = { 0.0f, 0.0f };
        for (size_t i = 1; i < vertCount; i++) {
            const float rotation = PI / vertCount * i;
            vertices[i] = {
                std::cos(rotation) * r,
                std::sin(rotation) * r
            };
        }

        return vertices;
    }

    auto rect(float w, float h) -> std::array<gl::Position2Df, 4> {
        const std::array<gl::Position2Df, 4> vertices = {
            -w/2,  h/2,
             w/2,  h/2,
             w/2, -h/2,
            -w/2, -h/2
        };

        return vertices;
    }
} // namespace primitives
