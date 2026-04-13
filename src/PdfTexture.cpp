#include "render/PdfTexture.hpp"

#include <glad/gl.h>

namespace no::render {

PdfTexture::PdfTexture() : texture_id_(0), width_(0), height_(0) {
}

PdfTexture::~PdfTexture() {
    Reset();
}

PdfTexture::PdfTexture(PdfTexture&& other) noexcept
    : texture_id_(other.texture_id_), width_(other.width_), height_(other.height_) {
    other.texture_id_ = 0;
    other.width_ = 0;
    other.height_ = 0;
}

PdfTexture& PdfTexture::operator=(PdfTexture&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    Reset();

    texture_id_ = other.texture_id_;
    width_ = other.width_;
    height_ = other.height_;

    other.texture_id_ = 0;
    other.width_ = 0;
    other.height_ = 0;

    return *this;
}

bool PdfTexture::UploadRGBA(int width, int height, const std::uint8_t* pixels) {
    if (width <= 0 || height <= 0 || pixels == nullptr) {
        return false;
    }

    if (texture_id_ == 0) {
        glGenTextures(1, &texture_id_);
    }

    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

    width_ = width;
    height_ = height;
    return glGetError() == GL_NO_ERROR;
}

void PdfTexture::Bind(unsigned int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
}

unsigned int PdfTexture::GetId() const {
    return texture_id_;
}

int PdfTexture::GetWidth() const {
    return width_;
}

int PdfTexture::GetHeight() const {
    return height_;
}

bool PdfTexture::IsValid() const {
    return texture_id_ != 0;
}

void PdfTexture::Reset() {
    if (texture_id_ != 0) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }

    width_ = 0;
    height_ = 0;
}

}  // namespace no::render
