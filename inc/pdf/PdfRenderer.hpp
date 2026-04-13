#pragma once

#include <cstdint>
#include <vector>

namespace no::pdf {

class PdfDocument;

/**
 * \brief Stores CPU-side pixel data for a rendered PDF page.
 */
struct RenderedPage {
    int width = 0;
    int height = 0;
    std::vector<std::uint8_t> pixels_rgba;
};

/**
 * \brief Renders PDF pages to RGBA pixel buffers using MuPDF.
 */
class PdfRenderer {
   public:
    /**
     * \brief Renders a PDF page.
     *
     * \param document The source document.
     * \param page_index The zero-based page index.
     * \param zoom The rendering zoom factor.
     * \param out_page The output pixel buffer.
     * \return True on success, false otherwise.
     */
    bool RenderPage(const PdfDocument& document, int page_index, float zoom, RenderedPage& out_page);
};

}  // namespace no::pdf
