#include "app/Application.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

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

void Application::OnKey(int key, int action, int /*mods*/) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
        case GLFW_KEY_ESCAPE:
            m_ShouldClose = true;
            break;

        case GLFW_KEY_N:
        case GLFW_KEY_J:
            NextPage();
            break;

        case GLFW_KEY_P:
        case GLFW_KEY_K:
            PreviousPage();
            break;

        case GLFW_KEY_F:
            FitCurrentPageToView();
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
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            m_Camera.BeginPan(mouse_x, mouse_y);
        } else if (action == GLFW_RELEASE) {
            m_Camera.EndPan();
        }
    }
}

void Application::OnMouseMove(double mouse_x, double mouse_y) {
    m_Camera.PanTo(mouse_x, mouse_y);
}

void Application::OnScroll(double /*xoffset*/, double yoffset, double mouse_x, double mouse_y) {
    m_Camera.ZoomAt(yoffset, mouse_x, mouse_y);
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

    m_CurrentPageIndex = page_index;
    return true;
}

}  // namespace no::app
