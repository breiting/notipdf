#pragma once

#include <cstdint>

namespace no::render {

/**
 * \brief Owns an OpenGL 2D texture for a rendered PDF page.
 *
 * The texture accepts raw RGBA8 pixel data and exposes only the operations
 * needed by the PDF viewer renderer.
 */
class PdfTexture {
   public:
    /**
     * \brief Constructs an empty texture object.
     */
    PdfTexture();

    /**
     * \brief Destroys the OpenGL texture.
     */
    ~PdfTexture();

    PdfTexture(const PdfTexture&) = delete;
    PdfTexture& operator=(const PdfTexture&) = delete;

    PdfTexture(PdfTexture&& other) noexcept;
    PdfTexture& operator=(PdfTexture&& other) noexcept;

    /**
     * \brief Uploads RGBA pixel data to the texture.
     *
     * \param width The texture width in pixels.
     * \param height The texture height in pixels.
     * \param pixels The source pixel pointer in RGBA8 format.
     * \return True on success, false otherwise.
     */
    bool UploadRGBA(int width, int height, const std::uint8_t* pixels);

    /**
     * \brief Binds the texture to the given texture unit.
     *
     * \param unit The texture unit index.
     */
    void Bind(unsigned int unit = 0) const;

    /**
     * \brief Returns the OpenGL texture handle.
     */
    unsigned int GetId() const;

    /**
     * \brief Returns the texture width in pixels.
     */
    int GetWidth() const;

    /**
     * \brief Returns the texture height in pixels.
     */
    int GetHeight() const;

/**
     * \brief Returns whether a valid OpenGL texture exists.
     */
    bool IsValid() const;

    /**
     * \brief Releases the OpenGL texture.
     * Must be called while GL context is still valid.
     */
    void Destroy();

private:
    void Reset();

    unsigned int m_TextureId;
    int m_Width;
    int m_Height;
};

}  // namespace no::render
