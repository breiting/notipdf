#pragma once

#include "pdf/PdfRenderer.hpp"

namespace no::ui {

class ViewerPanel {
   public:
    ViewerPanel() = default;
    ~ViewerPanel();

    ViewerPanel(const ViewerPanel&) = delete;
    ViewerPanel& operator=(const ViewerPanel&) = delete;

    bool EnsureTexture(const no::pdf::RenderedPage& page);
    void Draw(const no::pdf::RenderedPage& page);

   private:
    unsigned int texture_id_ = 0;
    int texture_width_ = 0;
    int texture_height_ = 0;
};

}  // namespace no::ui
