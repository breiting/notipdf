#pragma once

namespace no::render {

/**
 * \brief Stores and renders a single centered textured quad.
 *
 * The quad is defined in world space and centered at the origin.
 * It is used as the PDF page surface.
 */
class PdfQuad {
   public:
    /**
     * \brief Constructs an empty quad.
     */
    PdfQuad();

    /**
     * \brief Releases all OpenGL resources.
     */
    ~PdfQuad();

    PdfQuad(const PdfQuad&) = delete;
    PdfQuad& operator=(const PdfQuad&) = delete;

    /**
     * \brief Creates GPU buffers for the quad if needed.
     *
     * \return True on success, false otherwise.
     */
    bool Initialize();

    /**
     * \brief Updates the quad size in world units.
     *
     * \param width The quad width.
     * \param height The quad height.
     */
    void SetSize(float width, float height);

    /**
     * \brief Draws the quad.
     */
    void Render() const;

    /**
     * \brief Returns the quad width in world units.
     */
    float GetWidth() const;

    /**
     * \brief Returns the quad height in world units.
     */
    float GetHeight() const;

   private:
    void UploadVertexData();

    unsigned int m_Vao;
    unsigned int m_Vbo;
    unsigned int m_Ebo;
    float m_Width;
    float m_Height;
    bool m_Initialized;
};

}  // namespace no::render
