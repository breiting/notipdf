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

PdfQuad::PdfQuad() : m_Vao(0), m_Vbo(0), m_Ebo(0), m_Width(1.0f), m_Height(1.0f), m_Initialized(false) {
}

PdfQuad::~PdfQuad() {
    if (m_Ebo != 0) {
        glDeleteBuffers(1, &m_Ebo);
    }
    if (m_Vbo != 0) {
        glDeleteBuffers(1, &m_Vbo);
    }
    if (m_Vao != 0) {
        glDeleteVertexArrays(1, &m_Vao);
    }
}

bool PdfQuad::Initialize() {
    if (m_Initialized) {
        return true;
    }

    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);
    glGenBuffers(1, &m_Ebo);

    if (m_Vao == 0 || m_Vbo == 0 || m_Ebo == 0) {
        return false;
    }

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);

    constexpr unsigned int indices[6] = {0, 1, 2, 2, 3, 0};
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(sizeof(float) * 2));

    glBindVertexArray(0);

    UploadVertexData();
    m_Initialized = true;
    return glGetError() == GL_NO_ERROR;
}

void PdfQuad::SetSize(float width, float height) {
    m_Width = width;
    m_Height = height;

    if (m_Initialized) {
        UploadVertexData();
    }
}

void PdfQuad::Render() const {
    glBindVertexArray(m_Vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

float PdfQuad::GetWidth() const {
    return m_Width;
}

float PdfQuad::GetHeight() const {
    return m_Height;
}

void PdfQuad::UploadVertexData() {
    const float hw = m_Width * 0.5f;
    const float hh = m_Height * 0.5f;

    const Vertex vertices[4] = {
        {-hw, -hh, 0.0f, 1.0f},
        {hw, -hh, 1.0f, 1.0f},
        {hw, hh, 1.0f, 0.0f},
        {-hw, hh, 0.0f, 0.0f},
    };

    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

}  // namespace no::render
