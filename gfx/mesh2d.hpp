#pragma once

#include "gl/buffers.hpp"

struct MeshInfo {
    MeshInfo(gl::VertexArray va,
            gl::ArrayBuffer<GLfloat> vb,
            gl::IndexBuffer ib)
        : vb(std::move(vb)), va(va), ib(ib)
    {}

    gl::ArrayBuffer<GLfloat> vb;
    gl::VertexArray va;
    gl::IndexBuffer ib;
};

class Mesh2D {
public:
    explicit Mesh2D(const MeshInfo &info) {
        info_ = new MeshInfo(info);
    }
    Mesh2D(MeshInfo &info) {
        info_ = new MeshInfo(info);
    }
    ~Mesh2D() {
        if (info_)
            delete info_;
    }

    void render(GLenum drawMode) const {
        glBindVertexArray(info_->va);
        info_->vb.bind();
        info_->ib.bind();
        glDrawElements(drawMode, info_->ib.size, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        info_->ib.unbind();
        info_->vb.unbind();
    }
private:
    MeshInfo *info_;
};
