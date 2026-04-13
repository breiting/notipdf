#pragma once

#include <string>

#include "pdf/PdfDocument.hpp"
#include "pdf/PdfRenderer.hpp"
#include "render/Camera2D.hpp"
#include "render/PdfQuad.hpp"
#include "render/PdfTexture.hpp"
#include "render/PdfViewerRenderer.hpp"

namespace no::app {

/**
 * \brief Coordinates PDF loading, viewer state, rendering, and input handling.
 */
class Application {
   public:
    /**
     * \brief Initializes the application and loads the input PDF.
     *
     * \param pdf_path The input PDF path.
     * \param viewport_width The initial viewport width.
     * \param viewport_height The initial viewport height.
     * \return True on success, false otherwise.
     */
    bool Initialize(const std::string& pdf_path, int viewport_width, int viewport_height);

    /**
     * \brief Releases resources owned by the application.
     */
    void Shutdown();

    /**
     * \brief Handles keyboard input.
     *
     * \param key The GLFW key code.
     * \param action The GLFW action.
     * \param mods The GLFW modifier flags.
     */
    void OnKey(int key, int action, int mods);

    /**
     * \brief Returns whether the application requested shutdown.
     */
    bool ShouldClose() const;

    /**
     * \brief Updates the application state.
     *
     * \param dt The elapsed frame time in seconds.
     */
    void Update(float dt);

    /**
     * \brief Renders the current frame.
     */
    void Render();

    /**
     * \brief Handles viewport size changes.
     *
     * \param width The new width.
     * \param height The new height.
     */
    void SetViewportSize(int width, int height);

    /**
     * \brief Handles mouse button input.
     *
     * \param button The GLFW mouse button.
     * \param action The GLFW action.
     * \param mods The GLFW modifier flags.
     * \param mouse_x The current mouse X position.
     * \param mouse_y The current mouse Y position.
     */
    void OnMouseButton(int button, int action, int mods, double mouse_x, double mouse_y);

    /**
     * \brief Handles mouse move input.
     *
     * \param mouse_x The current mouse X position.
     * \param mouse_y The current mouse Y position.
     */
    void OnMouseMove(double mouse_x, double mouse_y);

    /**
     * \brief Handles mouse wheel input.
     *
     * \param xoffset The horizontal scroll offset.
     * \param yoffset The vertical scroll offset.
     * \param mouse_x The current mouse X position.
     * \param mouse_y The current mouse Y position.
     */
    void OnScroll(double xoffset, double yoffset, double mouse_x, double mouse_y);

   private:
    bool LoadPage(int page_index);
    void FitCurrentPageToView();

    pdf::PdfDocument document_;
    pdf::PdfRenderer pdf_renderer_;
    pdf::RenderedPage rendered_page_;

    render::PdfTexture page_texture_;
    render::PdfQuad page_quad_;
    render::PdfViewerRenderer viewer_renderer_;
    render::Camera2D camera_;

    int current_page_index_;
    bool initialized_ = false;
    bool should_close_ = false;
};

}  // namespace no::app
