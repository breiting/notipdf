#include "render/Camera2D.hpp"

#include <algorithm>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

namespace no::render {

namespace {
constexpr float kMinViewHeight = 0.05f;
constexpr float kMaxViewHeight = 100.0f;
constexpr float kZoomStep = 0.14f;
constexpr float kSmoothness = 14.0f;
constexpr float kFitMargin = 1.02f;
}  // namespace

Camera2D::Camera2D()
    : m_ViewportSize(1600, 1000),
      m_Position(0.0f, 0.0f),
      m_TargetPosition(0.0f, 0.0f),
      m_ViewHeight(1.2f),
      m_TargetViewHeight(1.2f),
      m_IsPanning(false),
      m_LastPanMouse(0.0, 0.0) {
}

void Camera2D::SetViewportSize(int width, int height) {
    m_ViewportSize.x = std::max(1, width);
    m_ViewportSize.y = std::max(1, height);
}

void Camera2D::Update(float dt) {
    const float alpha = 1.0f - std::exp(-kSmoothness * dt);
    m_Position += (m_TargetPosition - m_Position) * alpha;
    m_ViewHeight += (m_TargetViewHeight - m_ViewHeight) * alpha;
}

void Camera2D::FitToContent(float content_width, float content_height) {
    const float viewport_aspect = GetAspectRatio();
    const float content_aspect = content_width / content_height;

    float fitted_height = content_height;
    if (viewport_aspect < content_aspect) {
        fitted_height = content_width / viewport_aspect;
    }

    m_Position = glm::vec2(0.0f, 0.0f);
    m_TargetPosition = m_Position;

    m_ViewHeight = fitted_height * kFitMargin;
    m_TargetViewHeight = m_ViewHeight;
}

void Camera2D::BeginPan(double screen_x, double screen_y) {
    m_IsPanning = true;
    m_LastPanMouse = glm::dvec2(screen_x, screen_y);
}

void Camera2D::PanTo(double screen_x, double screen_y) {
    if (!m_IsPanning) {
        return;
    }

    const glm::dvec2 current(screen_x, screen_y);
    const glm::dvec2 delta = current - m_LastPanMouse;
    m_LastPanMouse = current;

    const float world_per_pixel_y = GetTargetViewHeight() / static_cast<float>(m_ViewportSize.y);
    const float world_per_pixel_x = GetTargetViewWidth() / static_cast<float>(m_ViewportSize.x);

    m_TargetPosition.x -= static_cast<float>(delta.x) * world_per_pixel_x;
    m_TargetPosition.y += static_cast<float>(delta.y) * world_per_pixel_y;
}

void Camera2D::EndPan() {
    m_IsPanning = false;
}

void Camera2D::ZoomAt(double scroll_delta, double screen_x, double screen_y) {
    if (std::abs(scroll_delta) < 1e-6) {
        return;
    }

    const glm::vec2 world_before = ScreenToWorld(screen_x, screen_y);

    const float zoom_factor = std::exp(-static_cast<float>(scroll_delta) * kZoomStep);
    m_TargetViewHeight = std::clamp(m_TargetViewHeight * zoom_factor, kMinViewHeight, kMaxViewHeight);

    const glm::vec2 world_after = ScreenToWorld(screen_x, screen_y);
    m_TargetPosition += (world_before - world_after);
}

glm::vec2 Camera2D::ScreenToWorld(double screen_x, double screen_y) const {
    const float ndc_x = (2.0f * static_cast<float>(screen_x) / static_cast<float>(m_ViewportSize.x)) - 1.0f;
    const float ndc_y = 1.0f - (2.0f * static_cast<float>(screen_y) / static_cast<float>(m_ViewportSize.y));

    const float half_w = GetTargetViewWidth() * 0.5f;
    const float half_h = GetTargetViewHeight() * 0.5f;

    return {m_TargetPosition.x + ndc_x * half_w, m_TargetPosition.y + ndc_y * half_h};
}

glm::mat4 Camera2D::GetViewMatrix() const {
    return glm::translate(glm::mat4(1.0f), glm::vec3(-m_Position.x, -m_Position.y, 0.0f));
}

glm::mat4 Camera2D::GetProjectionMatrix() const {
    const float half_w = GetCurrentViewWidth() * 0.5f;
    const float half_h = GetCurrentViewHeight() * 0.5f;

    return glm::ortho(-half_w, half_w, -half_h, half_h, -1.0f, 1.0f);
}

glm::ivec2 Camera2D::GetViewportSize() const {
    return m_ViewportSize;
}

glm::vec2 Camera2D::GetPosition() const {
    return m_Position;
}

float Camera2D::GetViewHeight() const {
    return m_ViewHeight;
}

float Camera2D::GetAspectRatio() const {
    return static_cast<float>(m_ViewportSize.x) / static_cast<float>(m_ViewportSize.y);
}

float Camera2D::GetCurrentViewWidth() const {
    return m_ViewHeight * GetAspectRatio();
}

float Camera2D::GetCurrentViewHeight() const {
    return m_ViewHeight;
}

float Camera2D::GetTargetViewWidth() const {
    return m_TargetViewHeight * GetAspectRatio();
}

float Camera2D::GetTargetViewHeight() const {
    return m_TargetViewHeight;
}

}  // namespace no::render
