#pragma once

#include <glm/glm.hpp>

namespace no::render {

class Camera2D;
class PdfQuad;
class PdfTexture;

/**
 * \brief Renders a textured PDF quad with a dedicated shader.
 */
class PdfViewerRenderer {
   public:
    /**
     * \brief Constructs an uninitialized renderer.
     */
    PdfViewerRenderer();

    /**
     * \brief Releases all OpenGL resources.
     */
    ~PdfViewerRenderer();

    PdfViewerRenderer(const PdfViewerRenderer&) = delete;
    PdfViewerRenderer& operator=(const PdfViewerRenderer&) = delete;

    /**
     * \brief Initializes shader resources.
     *
     * \return True on success, false otherwise.
     */
    bool Initialize();

    /**
     * \brief Sets the viewport size in pixels.
     *
     * \param width The viewport width.
     * \param height The viewport height.
     */
    void SetViewportSize(int width, int height);

    /**
     * \brief Clears the frame.
     */
    void BeginFrame() const;

    /**
     * \brief Draws the PDF page.
     *
     * \param camera The active 2D camera.
     * \param quad The page quad.
     * \param texture The page texture.
     */
    void Draw(const Camera2D& camera, const PdfQuad& quad, const PdfTexture& texture) const;

   private:
    unsigned int CompileShader(unsigned int type, const char* source) const;
    bool LinkProgram(unsigned int vertex_shader, unsigned int fragment_shader);

    unsigned int program_;
    int u_mvp_;
    int u_texture_;
    glm::ivec2 viewport_size_;
};

}  // namespace no::render
