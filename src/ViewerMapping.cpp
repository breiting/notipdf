#include "render/ViewerMapping.hpp"

#include <algorithm>

namespace no::render {

void ViewerMapping::SetPageBounds(float x0, float y0, float x1, float y1) {
    m_PageX0 = x0;
    m_PageY0 = y0;
    m_PageX1 = x1;
    m_PageY1 = y1;
}

void ViewerMapping::SetQuadSize(float quad_width, float quad_height) {
    m_QuadWidth = quad_width;
    m_QuadHeight = quad_height;
}

glm::vec2 ViewerMapping::WorldToQuadLocal(const glm::vec2& world) const {
    return ApplyInverseRotation(world);
}

bool ViewerMapping::IsInsidePage(const glm::vec2& world) const {
    const glm::vec2 local = WorldToQuadLocal(world);

    const float half_w = m_QuadWidth * 0.5f;
    const float half_h = m_QuadHeight * 0.5f;

    return local.x >= -half_w && local.x <= half_w && local.y >= -half_h && local.y <= half_h;
}

glm::vec2 ViewerMapping::ApplyForwardRotation(const glm::vec2& point) const {
    const float angle_rad = glm::radians(static_cast<float>(m_RotationDegrees));
    const float c = std::cos(angle_rad);
    const float s = std::sin(angle_rad);

    return {point.x * c - point.y * s, point.x * s + point.y * c};
}

pdf::PdfSelection ViewerMapping::MakeSelectionFromWorldDrag(int page_index, const glm::vec2& world_a,
                                                            const glm::vec2& world_b) const {
    const glm::vec2 a = WorldToQuadLocal(world_a);
    const glm::vec2 b = WorldToQuadLocal(world_b);

    const float half_w = m_QuadWidth * 0.5f;
    const float half_h = m_QuadHeight * 0.5f;

    const float ax = std::clamp(a.x, -half_w, half_w);
    const float ay = std::clamp(a.y, -half_h, half_h);
    const float bx = std::clamp(b.x, -half_w, half_w);
    const float by = std::clamp(b.y, -half_h, half_h);

    const float min_x = std::min(ax, bx);
    const float max_x = std::max(ax, bx);
    const float min_y = std::min(ay, by);
    const float max_y = std::max(ay, by);

    const float u0 = (min_x + half_w) / m_QuadWidth;
    const float u1 = (max_x + half_w) / m_QuadWidth;

    const float v0_bottom = (min_y + half_h) / m_QuadHeight;
    const float v1_bottom = (max_y + half_h) / m_QuadHeight;

    const float v0 = 1.0f - v1_bottom;
    const float v1 = 1.0f - v0_bottom;

    const float page_w = m_PageX1 - m_PageX0;
    const float page_h = m_PageY1 - m_PageY0;

    pdf::PdfSelection selection;
    selection.PageIndex = page_index;
    selection.X = m_PageX0 + u0 * page_w;
    selection.Y = m_PageY0 + v0 * page_h;
    selection.Width = (u1 - u0) * page_w;
    selection.Height = (v1 - v0) * page_h;
    return selection;
}

void ViewerMapping::SelectionToWorldRect(const pdf::PdfSelection& selection, glm::vec2& out_min,
                                         glm::vec2& out_max) const {
    const float half_w = m_QuadWidth * 0.5f;
    const float half_h = m_QuadHeight * 0.5f;

    const float page_w = m_PageX1 - m_PageX0;
    const float page_h = m_PageY1 - m_PageY0;

    const float u0 = (selection.X - m_PageX0) / page_w;
    const float u1 = (selection.X + selection.Width - m_PageX0) / page_w;

    const float v0 = (selection.Y - m_PageY0) / page_h;
    const float v1 = (selection.Y + selection.Height - m_PageY0) / page_h;

    const float x0 = u0 * m_QuadWidth - half_w;
    const float x1 = u1 * m_QuadWidth - half_w;

    const float y_top0 = half_h - v0 * m_QuadHeight;
    const float y_top1 = half_h - v1 * m_QuadHeight;

    const glm::vec2 p0 = ApplyForwardRotation(glm::vec2(x0, y_top0));
    const glm::vec2 p1 = ApplyForwardRotation(glm::vec2(x1, y_top0));
    const glm::vec2 p2 = ApplyForwardRotation(glm::vec2(x1, y_top1));
    const glm::vec2 p3 = ApplyForwardRotation(glm::vec2(x0, y_top1));

    const float min_x = std::min(std::min(p0.x, p1.x), std::min(p2.x, p3.x));
    const float max_x = std::max(std::max(p0.x, p1.x), std::max(p2.x, p3.x));
    const float min_y = std::min(std::min(p0.y, p1.y), std::min(p2.y, p3.y));
    const float max_y = std::max(std::max(p0.y, p1.y), std::max(p2.y, p3.y));

    out_min = glm::vec2(min_x, min_y);
    out_max = glm::vec2(max_x, max_y);
}

float ViewerMapping::GetPageX0() const {
    return m_PageX0;
}

float ViewerMapping::GetPageY0() const {
    return m_PageY0;
}

float ViewerMapping::GetPageX1() const {
    return m_PageX1;
}

float ViewerMapping::GetPageY1() const {
    return m_PageY1;
}

void ViewerMapping::SetRotationDegrees(int degrees) {
    m_RotationDegrees = degrees % 360;
    if (m_RotationDegrees < 0) {
        m_RotationDegrees += 360;
    }
}

glm::vec2 ViewerMapping::WorldToPage(const glm::vec2& world) const {
    const glm::vec2 local = WorldToQuadLocal(world);

    const float half_w = m_QuadWidth * 0.5f;
    const float half_h = m_QuadHeight * 0.5f;

    const float u = (local.x + half_w) / m_QuadWidth;
    const float v_bottom = (local.y + half_h) / m_QuadHeight;
    const float v = 1.0f - v_bottom;

    const float page_w = m_PageX1 - m_PageX0;
    const float page_h = m_PageY1 - m_PageY0;

    return {m_PageX0 + u * page_w, m_PageY0 + v * page_h};
}

glm::vec2 ViewerMapping::ApplyInverseRotation(const glm::vec2& point) const {
    const float angle_rad = glm::radians(-static_cast<float>(m_RotationDegrees));
    const float c = std::cos(angle_rad);
    const float s = std::sin(angle_rad);

    return {point.x * c - point.y * s, point.x * s + point.y * c};
}

}  // namespace no::render
