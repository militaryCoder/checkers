#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <GL/glew.h>

namespace gl {
    using VertexArray = GLuint;
    using DrawMode = GLenum;

    template <typename T>
    struct ArrayBuffer {
        ArrayBuffer() = default;
        ArrayBuffer(const ArrayBuffer &other) = default;
        ArrayBuffer(ArrayBuffer &&other) = default;

        ArrayBuffer(const std::vector<T> &data, DrawMode dmode)
        : drawMode_(dmode) {
            this->data(data);
        }

        void data(const std::vector<T> &data) {
            this->bind();
            glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data.size(), data.data(), drawMode_);
            this->unbind();
        }
        void bind() const {
            glBindBuffer(GL_ARRAY_BUFFER, this->id_);
        }
        void unbind() const {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

    private:
        GLint id_;
        DrawMode drawMode_;
    };

    struct IndexBuffer {
        IndexBuffer() = default;
        IndexBuffer(std::vector<GLuint> &indices,
                DrawMode dmode)
        : size(indices.size()) {
            glGenBuffers(1, &id_);
            this->bind();
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    this->size * sizeof(GLuint),
                    indices.data(),
                    dmode);
            this->unbind();
        }

        void data(const std::vector<GLuint> &data) {
            this->bind();
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint) * data.size(), data.data(), drawMode_);
            this->unbind();
        }

        void bind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->id_);
        }
        void unbind() const {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        size_t size;
    private:
        GLuint id_;
        DrawMode drawMode_;
    };
} // namespace gl
