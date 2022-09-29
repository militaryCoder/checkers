#pragma once

#include <memory>

#include "gl/types.hpp"
#include "mesh2d.hpp"

class MeshBox {
public:
    explicit MeshBox(Mesh2D *mesh)
        : mesh_(std::unique_ptr<Mesh2D>(mesh)),
        pos_({ .x = 0.0f, .y = 0.0f })
    {}

    void setPos(gl::Position2Df p) {
        pos_ = p;
    }
private:
    std::unique_ptr<Mesh2D> mesh_;
    gl::Position2Df pos_;
};
