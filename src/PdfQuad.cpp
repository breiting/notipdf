#include "render/PdfQuad.hpp"

#include <glad/gl.h>

namespace no::render {

namespace {
struct Vertex {
    float px;
    float py;
    float u;
    float v;
};
}  // namespace

PdfQuad::PdfQuad() : vao_(0), vbo_(0), ebo_(0), width_(1.0f), height_(1.0f), initialized_(false) {
}

PdfQuad::~PdfQuad() {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
    }
}

bool PdfQuad::Initialize() {
    if (initialized_) {
        return true;
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    if (vao_ == 0 || vbo_ == 0 || ebo_ == 0) {
        return false;
    }

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

    constexpr unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(float) * 2));

    glBindVertexArray(0);

    UploadVertexData();
    initialized_ = true;
    return glGetError() == GL_NO_ERROR;
}

void PdfQuad::SetSize(float width, float height) {
    width_ = width;
    height_ = height;

    if (initialized_) {
        UploadVertexData();
    }
}

void PdfQuad::Render() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

float PdfQuad::GetWidth() const {
    return width_;
}

float PdfQuad::GetHeight() const {
    return height_;
}

void PdfQuad::UploadVertexData() {
    const float hw = width_ * 0.5f;
    const float hh = height_ * 0.5f;

    const Vertex vertices[4] = {
        {-hw, -hh, 0.0f, 1.0f},
        {hw, -hh, 1.0f, 1.0f},
        {hw, hh, 1.0f, 0.0f},
        {-hw, hh, 0.0f, 0.0f},
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

}  // namespace no::render
