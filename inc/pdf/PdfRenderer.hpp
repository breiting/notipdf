#pragma once

#include <cstdint>
#include <vector>

namespace no::pdf {

class PdfDocument;

struct RenderedPage {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> pixels_rgba;
};

class PdfRenderer {
   public:
    bool RenderPage(const PdfDocument& document, int page_index, float zoom, RenderedPage& out_page);
};

}  // namespace no::pdf
