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
    : viewport_size_(1600, 1000),
      position_(0.0f, 0.0f),
      target_position_(0.0f, 0.0f),
      view_height_(1.2f),
      target_view_height_(1.2f),
      is_panning_(false),
      last_pan_mouse_(0.0, 0.0) {
}

void Camera2D::SetViewportSize(int width, int height) {
    viewport_size_.x = std::max(1, width);
    viewport_size_.y = std::max(1, height);
}

void Camera2D::Update(float dt) {
    const float alpha = 1.0f - std::exp(-kSmoothness * dt);
    position_ += (target_position_ - position_) * alpha;
    view_height_ += (target_view_height_ - view_height_) * alpha;
}

void Camera2D::FitToContent(float content_width, float content_height) {
    const float viewport_aspect = GetAspectRatio();
    const float content_aspect = content_width / content_height;

    float fitted_height = content_height;
    if (viewport_aspect < content_aspect) {
        fitted_height = content_width / viewport_aspect;
    }

    position_ = glm::vec2(0.0f, 0.0f);
    target_position_ = position_;

    view_height_ = fitted_height * kFitMargin;
    target_view_height_ = view_height_;
}

void Camera2D::BeginPan(double screen_x, double screen_y) {
    is_panning_ = true;
    last_pan_mouse_ = glm::dvec2(screen_x, screen_y);
}

void Camera2D::PanTo(double screen_x, double screen_y) {
    if (!is_panning_) {
        return;
    }

    const glm::dvec2 current(screen_x, screen_y);
    const glm::dvec2 delta = current - last_pan_mouse_;
    last_pan_mouse_ = current;

    const float world_per_pixel_y = GetTargetViewHeight() / static_cast<float>(viewport_size_.y);
    const float world_per_pixel_x = GetTargetViewWidth() / static_cast<float>(viewport_size_.x);

    target_position_.x -= static_cast<float>(delta.x) * world_per_pixel_x;
    target_position_.y += static_cast<float>(delta.y) * world_per_pixel_y;
}

void Camera2D::EndPan() {
    is_panning_ = false;
}

void Camera2D::ZoomAt(double scroll_delta, double screen_x, double screen_y) {
    if (std::abs(scroll_delta) < 1e-6) {
        return;
    }

    const glm::vec2 world_before = ScreenToWorld(screen_x, screen_y);

    const float zoom_factor = std::exp(-static_cast<float>(scroll_delta) * kZoomStep);
    target_view_height_ = std::clamp(target_view_height_ * zoom_factor, kMinViewHeight, kMaxViewHeight);

    const glm::vec2 world_after = ScreenToWorld(screen_x, screen_y);
    target_position_ += (world_before - world_after);
}

glm::vec2 Camera2D::ScreenToWorld(double screen_x, double screen_y) const {
    const float ndc_x = (2.0f * static_cast<float>(screen_x) / static_cast<float>(viewport_size_.x)) - 1.0f;
    const float ndc_y = 1.0f - (2.0f * static_cast<float>(screen_y) / static_cast<float>(viewport_size_.y));

    const float half_w = GetTargetViewWidth() * 0.5f;
    const float half_h = GetTargetViewHeight() * 0.5f;

    return {target_position_.x + ndc_x * half_w, target_position_.y + ndc_y * half_h};
}

glm::mat4 Camera2D::GetViewMatrix() const {
    return glm::translate(glm::mat4(1.0f), glm::vec3(-position_.x, -position_.y, 0.0f));
}

glm::mat4 Camera2D::GetProjectionMatrix() const {
    const float half_w = GetCurrentViewWidth() * 0.5f;
    const float half_h = GetCurrentViewHeight() * 0.5f;

    return glm::ortho(-half_w, half_w, -half_h, half_h, -1.0f, 1.0f);
}

glm::ivec2 Camera2D::GetViewportSize() const {
    return viewport_size_;
}

glm::vec2 Camera2D::GetPosition() const {
    return position_;
}

float Camera2D::GetViewHeight() const {
    return view_height_;
}

float Camera2D::GetAspectRatio() const {
    return static_cast<float>(viewport_size_.x) / static_cast<float>(viewport_size_.y);
}

float Camera2D::GetCurrentViewWidth() const {
    return view_height_ * GetAspectRatio();
}

float Camera2D::GetCurrentViewHeight() const {
    return view_height_;
}

float Camera2D::GetTargetViewWidth() const {
    return target_view_height_ * GetAspectRatio();
}

float Camera2D::GetTargetViewHeight() const {
    return target_view_height_;
}

}  // namespace no::render
