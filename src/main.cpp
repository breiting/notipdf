#include <CLI/CLI.hpp>

#include "app/Application.hpp"
#include "ui/Window.hpp"

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
    if (!app.Initialize(input_pdf)) {
        window.Destroy();
        return 1;
    }

    while (window.PollEvents()) {
        window.BeginFrame();

        app.Update(1.0 / 60.0);
        app.DrawUI();

        window.EndFrame();
        window.SwapBuffers();
    }

    app.Shutdown();
    window.Destroy();
    return 0;
}
