#pragma once

#include <glm/glm.hpp>

namespace no::render {

/**
 * \brief Implements a lightweight 2D camera for PDF viewing.
 *
 * The camera supports smooth zooming, panning, and viewport-aware
 * coordinate conversion. The camera is centered on a target position
 * and uses an orthographic projection.
 */
class Camera2D {
   public:
    /**
     * \brief Constructs a camera with default viewport and view size.
     */
    Camera2D();

    /**
     * \brief Sets the viewport size in pixels.
     *
     * \param width The viewport width in pixels.
     * \param height The viewport height in pixels.
     */
    void SetViewportSize(int width, int height);

    /**
     * \brief Updates the camera interpolation state.
     *
     * \param dt The elapsed frame time in seconds.
     */
    void Update(float dt);

    /**
     * \brief Resets the camera to show a world-space rectangle fully.
     *
     * \param content_width The content width in world units.
     * \param content_height The content height in world units.
     */
    void FitToContent(float content_width, float content_height);

    /**
     * \brief Starts a panning interaction.
     *
     * \param screen_x The mouse X position in screen coordinates.
     * \param screen_y The mouse Y position in screen coordinates.
     */
    void BeginPan(double screen_x, double screen_y);

    /**
     * \brief Updates the camera target position from mouse drag input.
     *
     * \param screen_x The current mouse X position in screen coordinates.
     * \param screen_y The current mouse Y position in screen coordinates.
     */
    void PanTo(double screen_x, double screen_y);

    /**
     * \brief Ends the current panning interaction.
     */
    void EndPan();

    /**
     * \brief Applies zoom at the given screen-space pivot.
     *
     * Positive offsets zoom in, negative offsets zoom out.
     *
     * \param scroll_delta The vertical mouse wheel delta.
     * \param screen_x The mouse X position in screen coordinates.
     * \param screen_y The mouse Y position in screen coordinates.
     */
    void ZoomAt(double scroll_delta, double screen_x, double screen_y);

    /**
     * \brief Converts screen coordinates to world coordinates.
     *
     * \param screen_x The X position in screen coordinates.
     * \param screen_y The Y position in screen coordinates.
     * \return The corresponding world-space position.
     */
    glm::vec2 ScreenToWorld(double screen_x, double screen_y) const;

    /**
     * \brief Returns the view matrix.
     *
     * \return The camera view matrix.
     */
    glm::mat4 GetViewMatrix() const;

    /**
     * \brief Returns the orthographic projection matrix.
     *
     * \return The camera projection matrix.
     */
    glm::mat4 GetProjectionMatrix() const;

    /**
     * \brief Returns the current viewport size.
     */
    glm::ivec2 GetViewportSize() const;

    /**
     * \brief Returns the current camera position.
     */
    glm::vec2 GetPosition() const;

    /**
     * \brief Returns the current visible world height.
     */
    float GetViewHeight() const;

   private:
    float GetAspectRatio() const;
    float GetCurrentViewWidth() const;
    float GetCurrentViewHeight() const;
    float GetTargetViewWidth() const;
    float GetTargetViewHeight() const;

    glm::ivec2 viewport_size_;
    glm::vec2 position_;
    glm::vec2 target_position_;

    float view_height_;
    float target_view_height_;

    bool is_panning_;
    glm::dvec2 last_pan_mouse_;
};

}  // namespace no::render
