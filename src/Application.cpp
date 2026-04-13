#include "app/Application.hpp"

#include <imgui.h>

namespace no::app {

bool Application::Initialize(const std::string& pdf_path) {
    input_path_ = pdf_path;

    if (!document_.Open(pdf_path)) {
        return false;
    }

    if (document_.GetPageCount() <= 0) {
        return false;
    }

    loaded_ = LoadFirstPage();
    return loaded_;
}

void Application::Shutdown() {
    document_.Close();
    loaded_ = false;
}

void Application::Update(double /*dt*/) {
}

void Application::DrawUI() {
    ImGui::Begin("Document");
    ImGui::Text("File: %s", input_path_.c_str());
    ImGui::Text("Pages: %d", document_.GetPageCount());
    ImGui::End();

    if (loaded_) {
        viewer_panel_.Draw(rendered_page_);
    } else {
        ImGui::Begin("PDF");
        ImGui::TextUnformatted("No PDF loaded.");
        ImGui::End();
    }
}

bool Application::LoadFirstPage() {
    if (!renderer_.RenderPage(document_, 0, 2.0f, rendered_page_)) {
        return false;
    }

    if (!viewer_panel_.EnsureTexture(rendered_page_)) {
        return false;
    }

    return true;
}

}  // namespace no::app
