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
    void Draw(const Camera2D& camera, const PdfQuad& quad, const PdfTexture& texture, int rotation_degrees) const;

    /**
     * \brief Draws a rectangle outline in world space.
     *
     * \param camera The active camera.
     * \param min_corner The minimum world corner.
     * \param max_corner The maximum world corner.
     */
    void DrawRectOutline(const Camera2D& camera, const glm::vec2& min_corner, const glm::vec2& max_corner) const;

   private:
    unsigned int CompileShader(unsigned int type, const char* source) const;
    bool LinkProgram(unsigned int vertex_shader, unsigned int fragment_shader);

    unsigned int m_Program;
    int m_UMvp;
    int m_UTexture;
    glm::ivec2 m_ViewportSize;
};

}  // namespace no::render
