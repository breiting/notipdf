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
    unsigned int m_TextureId = 0;
    int m_TextureWidth = 0;
    int m_TextureHeight = 0;
};

}  // namespace no::ui
