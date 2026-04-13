#pragma once

#include <string>

#include "pdf/PdfDocument.hpp"
#include "pdf/PdfRenderer.hpp"
#include "ui/ViewerPanel.hpp"

namespace no::app {

class Application {
   public:
    bool Initialize(const std::string& pdf_path);
    void Shutdown();

    void Update(double dt);
    void DrawUI();

   private:
    bool LoadFirstPage();

    no::pdf::PdfDocument document_;
    no::pdf::PdfRenderer renderer_;
    no::pdf::RenderedPage rendered_page_;
    no::ui::ViewerPanel viewer_panel_;

    std::string input_path_;
    bool loaded_ = false;
};

}  // namespace no::app
