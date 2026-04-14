#pragma once

#include <glm/glm.hpp>

namespace no::render {

class Camera2D;

/**
 * \brief Renders simple colored rectangle outlines in world space.
 */
class LineRenderer {
   public:
    /**
     * \brief Constructs an uninitialized line renderer.
     */
    LineRenderer();

    /**
     * \brief Releases all OpenGL resources.
     */
    ~LineRenderer();

    LineRenderer(const LineRenderer&) = delete;
    LineRenderer& operator=(const LineRenderer&) = delete;

    /**
     * \brief Initializes shader and buffer resources.
     *
     * \return True on success, false otherwise.
     */
    bool Initialize();

    /**
     * \brief Releases all OpenGL resources.
     */
    void Destroy();

    /**
     * \brief Draws a rectangle outline in world space.
     *
     * \param camera The active camera.
     * \param min_corner The minimum world-space corner.
     * \param max_corner The maximum world-space corner.
     * \param color The rectangle color as RGBA.
     */
    void DrawRectOutline(const Camera2D& camera, const glm::vec2& min_corner, const glm::vec2& max_corner,
                         const glm::vec4& color) const;

   private:
    unsigned int CompileShader(unsigned int type, const char* source) const;
    bool LinkProgram(unsigned int vertex_shader, unsigned int fragment_shader);

    unsigned int m_Program = 0;
    unsigned int m_Vao = 0;
    unsigned int m_Vbo = 0;

    int m_UniformMvp = -1;
    int m_UniformColor = -1;
};

}  // namespace no::render
