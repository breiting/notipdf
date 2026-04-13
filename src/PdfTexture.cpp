#include "render/PdfTexture.hpp"

#include <glad/gl.h>

namespace no::render {

PdfTexture::PdfTexture() : m_TextureId(0), m_Width(0), m_Height(0) {
}

PdfTexture::~PdfTexture() {
    Reset();
}

PdfTexture::PdfTexture(PdfTexture&& other) noexcept
    : m_TextureId(other.m_TextureId), m_Width(other.m_Width), m_Height(other.m_Height) {
    other.m_TextureId = 0;
    other.m_Width = 0;
    other.m_Height = 0;
}

PdfTexture& PdfTexture::operator=(PdfTexture&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    Reset();

    m_TextureId = other.m_TextureId;
    m_Width = other.m_Width;
    m_Height = other.m_Height;

    other.m_TextureId = 0;
    other.m_Width = 0;
    other.m_Height = 0;

    return *this;
}

bool PdfTexture::UploadRGBA(int width, int height, const std::uint8_t* pixels) {
    if (width <= 0 || height <= 0 || pixels == nullptr) {
        return false;
    }

    if (m_TextureId == 0) {
        glGenTextures(1, &m_TextureId);
    }

    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_Width = width;
    m_Height = height;
    return glGetError() == GL_NO_ERROR;
}

void PdfTexture::Bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
}

unsigned int PdfTexture::GetId() const {
    return m_TextureId;
}

int PdfTexture::GetWidth() const {
    return m_Width;
}

int PdfTexture::GetHeight() const {
    return m_Height;
}

bool PdfTexture::IsValid() const {
    return m_TextureId != 0;
}

void PdfTexture::Reset() {
    if (m_TextureId != 0) {
        glDeleteTextures(1, &m_TextureId);
        m_TextureId = 0;
    }

    m_Width = 0;
    m_Height = 0;
}

}  // namespace no::render
