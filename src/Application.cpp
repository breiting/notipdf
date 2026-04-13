#include "app/Application.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace no::app {

bool Application::Initialize(const std::string& pdf_path, int viewport_width, int viewport_height) {
    if (!document_.Open(pdf_path)) {
        return false;
    }

    if (document_.GetPageCount() <= 0) {
        return false;
    }

    if (!viewer_renderer_.Initialize()) {
        return false;
    }

    if (!page_quad_.Initialize()) {
        return false;
    }

    SetViewportSize(viewport_width, viewport_height);

    if (!LoadPage(0)) {
        return false;
    }

    initialized_ = true;
    return true;
}

void Application::Shutdown() {
    document_.Close();
    initialized_ = false;
}

void Application::OnKey(int key, int action, int /*mods*/) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
        case GLFW_KEY_Q:
            should_close_ = true;
            break;

        case GLFW_KEY_F:
            FitCurrentPageToView();
            break;

        default:
            break;
    }
}

bool Application::ShouldClose() const {
    return should_close_;
}

void Application::FitCurrentPageToView() {
    camera_.FitToContent(page_quad_.GetWidth(), page_quad_.GetHeight());
}

void Application::Update(float dt) {
    if (!initialized_) {
        return;
    }

    camera_.Update(dt);
}

void Application::Render() {
    viewer_renderer_.BeginFrame();

    if (!initialized_) {
        return;
    }

    viewer_renderer_.Draw(camera_, page_quad_, page_texture_);
}

void Application::SetViewportSize(int width, int height) {
    viewer_renderer_.SetViewportSize(width, height);
    camera_.SetViewportSize(width, height);
}

void Application::OnMouseButton(int button, int action, int /*mods*/, double mouse_x, double mouse_y) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            camera_.BeginPan(mouse_x, mouse_y);
        } else if (action == GLFW_RELEASE) {
            camera_.EndPan();
        }
    }
}

void Application::OnMouseMove(double mouse_x, double mouse_y) {
    camera_.PanTo(mouse_x, mouse_y);
}

void Application::OnScroll(double /*xoffset*/, double yoffset, double mouse_x, double mouse_y) {
    camera_.ZoomAt(yoffset, mouse_x, mouse_y);
}

bool Application::LoadPage(int page_index) {
    if (!pdf_renderer_.RenderPage(document_, page_index, 2.0f, rendered_page_)) {
        return false;
    }

    if (!page_texture_.UploadRGBA(rendered_page_.width, rendered_page_.height, rendered_page_.pixels_rgba.data())) {
        return false;
    }

    const float aspect = static_cast<float>(rendered_page_.width) / static_cast<float>(rendered_page_.height);

    page_quad_.SetSize(aspect, 1.0f);
    FitCurrentPageToView();

    current_page_index_ = page_index;
    return true;
}

}  // namespace no::app
