#pragma once

#include <optional>
#include <string>

#include "app/AppConfig.hpp"
#include "app/ConfigService.hpp"
#include "app/ExportDialogState.hpp"
#include "app/ViewerMode.hpp"
#include "pdf/MetaWriter.hpp"
#include "pdf/PdfDocument.hpp"
#include "pdf/PdfExporter.hpp"
#include "pdf/PdfRenderer.hpp"
#include "pdf/PdfSelection.hpp"
#include "render/Camera2D.hpp"
#include "render/LineRenderer.hpp"
#include "render/PdfQuad.hpp"
#include "render/PdfTexture.hpp"
#include "render/PdfViewerRenderer.hpp"
#include "render/ViewerMapping.hpp"

namespace no::app {

/**
 * \brief Coordinates PDF loading, viewer state, rendering, and input handling.
 */
class Application {
   public:
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
     * \brief Returns the current window title suffix for UI feedback.
     *
     * \return A short string describing the current mode and page state.
     */
    std::string GetWindowTitle() const;

    /**
     * \brief Handles keyboard input.
     *
     * \param key The GLFW key code.
     * \param action The GLFW action.
     * \param mods The GLFW modifier flags.
     */
    void OnKey(int key, int action, int mods);

    /**
     * \brief Draws transient ImGui UI such as modal dialogs.
     */
    void DrawGui();

    /**
     * \brief Returns whether the application requested shutdown.
     */
    bool ShouldClose() const;

    /**
     * \brief Returns the active export aspect ratio.
     *
     * \return Width divided by height for the active export preset.
     */
    float GetActiveExportAspectRatio() const;

    /**
     * \brief Adjusts a drag point so that the selection keeps a fixed aspect ratio.
     *
     * \param anchor_world The drag start point in world space.
     * \param current_world The current drag point in world space.
     * \param aspect_ratio The target width/height ratio.
     * \return The corrected drag point in world space.
     */
    glm::vec2 ApplyAspectLock(const glm::vec2& anchor_world, const glm::vec2& current_world, float aspect_ratio) const;

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
     * \brief Loads the next page if available.
     */
    void NextPage();

    /**
     * \brief Loads the previous page if available.
     */
    void PreviousPage();

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
    void ToggleViewerMode();
    void DrawSelectionOverlays() const;
    void OpenExportDialog();
    void DrawExportDialog();
    bool ConfirmExport();
    void ClearSelections();
    void MoveSelectionTo(const glm::vec2& current_world);
    image::ImageOptimizationSettings BuildImageOptimizationSettings() const;
    glm::vec2 GetEffectiveSelectionPoint(const glm::vec2& world) const;

    static std::string SanitizeFileName(const std::string& value);
    static const char* GetVoiceName(int index);

    pdf::PdfDocument m_Document;
    pdf::PdfRenderer m_PdfRenderer;
    pdf::RenderedPage m_RenderedPage;

    render::PdfTexture m_PageTexture;
    render::PdfQuad m_PageQuad;
    render::PdfViewerRenderer m_ViewerRenderer;
    render::Camera2D m_Camera;
    render::LineRenderer m_LineRenderer;

    ViewerMode m_ViewerMode = ViewerMode::Pan;

    int m_CurrentPageIndex = 0;
    bool m_Initialized = false;
    bool m_ShouldClose = false;

    render::ViewerMapping m_ViewerMapping;

    std::optional<pdf::PdfSelection> m_Selection;

    bool m_IsSelecting = false;
    bool m_IsMovingSelection = false;
    bool m_IsAspectLocked = false;
    glm::vec2 m_SelectionStartWorld = glm::vec2(0.0f);
    glm::vec2 m_SelectionCurrentWorld = glm::vec2(0.0f);

    glm::vec2 m_MoveStartWorld = glm::vec2(0.0f);
    pdf::PdfSelection m_MoveStartSelection;

    no::pdf::PdfExporter m_PdfExporter;
    pdf::MetaWriter m_MetaWriter;
    ExportDialogState m_ExportDialogState;
    bool m_SetFocusOnExportDialog = false;
    AppConfig m_Config;
    ConfigService m_ConfigService;
};

}  // namespace no::app
