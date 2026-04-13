#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>

#include <CLI/CLI.hpp>

#include "app/Application.hpp"
#include "ui/Window.hpp"

namespace {
no::app::Application* g_app = nullptr;
no::ui::Window* g_window = nullptr;

void KeyCallback(GLFWwindow* native, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(native, key, scancode, action, mods);

    if (g_app == nullptr) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) {
        return;
    }

    g_app->OnKey(key, action, mods);
}
void MouseButtonCallback(GLFWwindow* native, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(native, button, action, mods);

    if (g_app == nullptr || g_window == nullptr) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos(native, &x, &y);
    g_app->OnMouseButton(button, action, mods, x, y);
}

void CursorPosCallback(GLFWwindow* native, double x, double y) {
    ImGui_ImplGlfw_CursorPosCallback(native, x, y);

    if (g_app == nullptr) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    g_app->OnMouseMove(x, y);
}

void ScrollCallback(GLFWwindow* native, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(native, xoffset, yoffset);

    if (g_app == nullptr || g_window == nullptr) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) {
        return;
    }

    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos(native, &x, &y);
    g_app->OnScroll(xoffset, yoffset, x, y);
}

void FramebufferSizeCallback(GLFWwindow* native, int width, int height) {
    (void)native;
    if (g_app != nullptr) {
        g_app->SetViewportSize(width, height);
    }
}

void CharCallback(GLFWwindow* native, unsigned int c) {
    ImGui_ImplGlfw_CharCallback(native, c);
}
}  // namespace

int main(int argc, char** argv) {
    CLI::App cli{"notipdf"};
    std::string input_pdf;
    cli.add_option("input", input_pdf, "Input PDF")->required()->check(CLI::ExistingFile);
    CLI11_PARSE(cli, argc, argv);

    no::ui::Window window;

    no::ui::Window::CreateInfo ci;
    ci.width = 1600;
    ci.height = 1000;
    ci.title = "notipdf";

    if (!window.Create(ci)) {
        return 1;
    }

    no::app::Application app;
    if (!app.Initialize(input_pdf, ci.width, ci.height)) {
        window.Destroy();
        return 1;
    }

    g_app = &app;
    g_window = &window;

    GLFWwindow* native = window.GetNative();
    glfwSetKeyCallback(native, KeyCallback);
    glfwSetMouseButtonCallback(native, MouseButtonCallback);
    glfwSetCursorPosCallback(native, CursorPosCallback);
    glfwSetScrollCallback(native, ScrollCallback);
    glfwSetFramebufferSizeCallback(native, FramebufferSizeCallback);
    glfwSetCharCallback(native, CharCallback);

    double last_time = glfwGetTime();

    while (window.PollEvents()) {
        if (app.ShouldClose()) break;

        const double current_time = glfwGetTime();
        const float dt = static_cast<float>(current_time - last_time);
        last_time = current_time;

        app.Update(dt);
        app.Render();

        window.BeginFrame();
        app.DrawGui();
        window.EndFrame();

        glfwSetWindowTitle(native, app.GetWindowTitle().c_str());
        window.SwapBuffers();
    }

    app.Shutdown();
    window.Destroy();
    return 0;
}
