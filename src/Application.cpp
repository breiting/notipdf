#include "app/Application.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>

namespace no::app {

bool Application::Initialize(const std::string& pdf_path, int viewport_width, int viewport_height) {
    if (!m_Document.Open(pdf_path)) {
        return false;
    }

    if (m_Document.GetPageCount() <= 0) {
        return false;
    }

    if (!m_ViewerRenderer.Initialize()) {
        return false;
    }

    if (!m_LineRenderer.Initialize()) {
        return false;
    }

    if (!m_PageQuad.Initialize()) {
        return false;
    }

    SetViewportSize(viewport_width, viewport_height);

    if (!LoadPage(0)) {
        return false;
    }

    m_Initialized = true;
    return true;
}

void Application::Shutdown() {
    m_Document.Close();
    m_Initialized = false;
}

std::string Application::GetWindowTitle() const {
    std::ostringstream ss;
    ss << "notipdf"
       << " | mode: " << (m_ViewerMode == ViewerMode::Pan ? "PAN" : "SELECT") << " | page: " << (m_CurrentPageIndex + 1)
       << "/" << m_Document.GetPageCount();
    return ss.str();
}

void Application::ToggleViewerMode() {
    if (m_ViewerMode == ViewerMode::Pan) {
        m_ViewerMode = ViewerMode::Select;
        m_Camera.EndPan();
    } else {
        m_ViewerMode = ViewerMode::Pan;
        m_IsSelecting = false;
    }
}

void Application::OnKey(int key, int action, int /*mods*/) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
        case GLFW_KEY_ESCAPE:
            m_ShouldClose = true;
            break;

        case GLFW_KEY_SPACE:
            ToggleViewerMode();
            break;

        case GLFW_KEY_N:
        case GLFW_KEY_J:
            NextPage();
            break;

        case GLFW_KEY_P:
        case GLFW_KEY_K:
            PreviousPage();
            break;

        case GLFW_KEY_DELETE:
        case GLFW_KEY_BACKSPACE:
            UndoLastSelection();
            break;

        case GLFW_KEY_E:
            ExportLastSelection();
            break;

        case GLFW_KEY_F:
            FitCurrentPageToView();
            break;

        case GLFW_KEY_A:
            m_IsShiftDown = !m_IsShiftDown;
            break;

        default:
            break;
    }
}

bool Application::ShouldClose() const {
    return m_ShouldClose;
}

void Application::FitCurrentPageToView() {
    m_Camera.FitToContent(m_PageQuad.GetWidth(), m_PageQuad.GetHeight());
}

void Application::UndoLastSelection() {
    if (!m_Selections.empty()) {
        m_Selections.pop_back();
    }
}

void Application::ExportLastSelection() {
    if (m_Selections.empty()) return;

    const auto& selection = m_Selections.back();

    const std::filesystem::path output = std::filesystem::current_path() / "export.pdf";

    no::pdf::PdfExporter exporter;

    exporter.Export(m_Document, selection, output, no::pdf::ExportPreset::InkPad4Landscape);
    std::cout << "Exported PDF to: " << output << '\n';
}

void Application::Update(float dt) {
    if (!m_Initialized) {
        return;
    }

    m_Camera.Update(dt);
}

void Application::Render() {
    m_ViewerRenderer.BeginFrame();

    if (!m_Initialized) {
        return;
    }

    m_ViewerRenderer.Draw(m_Camera, m_PageQuad, m_PageTexture);
    DrawSelectionOverlays();
}

void Application::DrawSelectionOverlays() const {
    const glm::vec4 persistent_color(0.95f, 0.25f, 0.2f, 1.0f);
    const glm::vec4 preview_color(0.2f, 0.9f, 0.3f, 1.0f);

    for (const pdf::PdfSelection& selection : m_Selections) {
        glm::vec2 min_corner;
        glm::vec2 max_corner;
        m_ViewerMapping.SelectionToWorldRect(selection, min_corner, max_corner);
        m_LineRenderer.DrawRectOutline(m_Camera, min_corner, max_corner, persistent_color);
    }

    if (m_IsSelecting) {
        const pdf::PdfSelection preview = m_ViewerMapping.MakeSelectionFromWorldDrag(
            m_CurrentPageIndex, m_SelectionStartWorld, m_SelectionCurrentWorld);

        if (preview.Width > 0.0f && preview.Height > 0.0f) {
            glm::vec2 min_corner;
            glm::vec2 max_corner;
            m_ViewerMapping.SelectionToWorldRect(preview, min_corner, max_corner);
            m_LineRenderer.DrawRectOutline(m_Camera, min_corner, max_corner, preview_color);
        }
    }
}

void Application::SetViewportSize(int width, int height) {
    m_ViewerRenderer.SetViewportSize(width, height);
    m_Camera.SetViewportSize(width, height);
}

void Application::NextPage() {
    if (!m_Document.IsOpen()) {
        return;
    }

    const int next_page = m_CurrentPageIndex + 1;
    if (next_page >= m_Document.GetPageCount()) {
        return;
    }

    LoadPage(next_page);
}

void Application::PreviousPage() {
    if (!m_Document.IsOpen()) {
        return;
    }

    const int previous_page = m_CurrentPageIndex - 1;
    if (previous_page < 0) {
        return;
    }

    LoadPage(previous_page);
}

void Application::OnMouseButton(int button, int action, int /*mods*/, double mouse_x, double mouse_y) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    const glm::vec2 world = m_Camera.ScreenToWorld(mouse_x, mouse_y);

    if (m_ViewerMode == ViewerMode::Pan) {
        if (action == GLFW_PRESS) {
            m_Camera.BeginPan(mouse_x, mouse_y);
        } else if (action == GLFW_RELEASE) {
            m_Camera.EndPan();
        }
        return;
    }

    if (m_ViewerMode != ViewerMode::Select) {
        return;
    }

    if (action == GLFW_PRESS) {
        if (m_ViewerMapping.IsInsidePage(world)) {
            m_IsSelecting = true;
            m_SelectionStartWorld = world;
            m_SelectionCurrentWorld = world;
        }
        return;
    }

    if (action == GLFW_RELEASE) {
        if (!m_IsSelecting) {
            return;
        }

        glm::vec2 final_world = world;

        if (m_IsShiftDown) {
            final_world = ApplyAspectLock(m_SelectionStartWorld, world, GetActiveExportAspectRatio());
        }

        m_SelectionCurrentWorld = final_world;

        const pdf::PdfSelection selection = m_ViewerMapping.MakeSelectionFromWorldDrag(
            m_CurrentPageIndex, m_SelectionStartWorld, m_SelectionCurrentWorld);

        if (selection.Width > 4.0f && selection.Height > 4.0f) {
            m_Selections.push_back(selection);
        }

        m_IsSelecting = false;
    }
}

void Application::OnMouseMove(double mouse_x, double mouse_y) {
    if (m_ViewerMode == ViewerMode::Pan) {
        m_Camera.PanTo(mouse_x, mouse_y);
        return;
    }

    const glm::vec2 world = m_Camera.ScreenToWorld(mouse_x, mouse_y);
    if (m_IsShiftDown) {
        m_SelectionCurrentWorld = ApplyAspectLock(m_SelectionStartWorld, world, GetActiveExportAspectRatio());
    } else {
        m_SelectionCurrentWorld = world;
    }
}

void Application::OnScroll(double /*xoffset*/, double yoffset, double mouse_x, double mouse_y) {
    m_Camera.ZoomAt(yoffset, mouse_x, mouse_y);
}

float Application::GetActiveExportAspectRatio() const {
    // TODO: fix it
    return 1872.0f / 1404.0f;
}

glm::vec2 Application::ApplyAspectLock(const glm::vec2& anchor_world, const glm::vec2& current_world,
                                       float aspect_ratio) const {
    const glm::vec2 delta = current_world - anchor_world;

    const float sign_x = (delta.x < 0.0f) ? -1.0f : 1.0f;
    const float sign_y = (delta.y < 0.0f) ? -1.0f : 1.0f;

    const float abs_dx = std::abs(delta.x);
    const float abs_dy = std::abs(delta.y);

    if (abs_dx < 1e-6f && abs_dy < 1e-6f) {
        return current_world;
    }

    float locked_w = abs_dx;
    float locked_h = abs_dy;

    if (abs_dy < 1e-6f) {
        locked_h = locked_w / aspect_ratio;
    } else if (abs_dx < 1e-6f) {
        locked_w = locked_h * aspect_ratio;
    } else {
        const float current_aspect = abs_dx / abs_dy;

        if (current_aspect > aspect_ratio) {
            locked_h = abs_dx / aspect_ratio;
            locked_w = abs_dx;
        } else {
            locked_w = abs_dy * aspect_ratio;
            locked_h = abs_dy;
        }
    }

    return {anchor_world.x + sign_x * locked_w, anchor_world.y + sign_y * locked_h};
}

bool Application::LoadPage(int page_index) {
    if (!m_PdfRenderer.RenderPage(m_Document, page_index, 2.0f, m_RenderedPage)) {
        return false;
    }

    if (!m_PageTexture.UploadRGBA(m_RenderedPage.width, m_RenderedPage.height, m_RenderedPage.pixels_rgba.data())) {
        return false;
    }

    const float aspect = static_cast<float>(m_RenderedPage.width) / static_cast<float>(m_RenderedPage.height);

    m_PageQuad.SetSize(aspect, 1.0f);
    FitCurrentPageToView();

    float page_x0 = 0.0f;
    float page_y0 = 0.0f;
    float page_x1 = 0.0f;
    float page_y1 = 0.0f;
    if (!m_Document.GetPageBounds(page_index, page_x0, page_y0, page_x1, page_y1)) {
        return false;
    }

    m_ViewerMapping.SetPageBounds(page_x0, page_y0, page_x1, page_y1);
    m_ViewerMapping.SetQuadSize(m_PageQuad.GetWidth(), m_PageQuad.GetHeight());

    m_CurrentPageIndex = page_index;
    m_Selections.clear();
    m_IsSelecting = false;

    return true;
}
}  // namespace no::app
