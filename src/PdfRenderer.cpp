#include "pdf/PdfRenderer.hpp"

#include <mupdf/fitz.h>

#include "pdf/PdfDocument.hpp"

namespace no::pdf {

bool PdfRenderer::RenderPage(const PdfDocument& document, int page_index, float zoom, RenderedPage& out_page) {
    if (!document.IsOpen()) {
        return false;
    }

    fz_context* ctx = document.GetContext();
    fz_document* doc = document.GetHandle();

    fz_page* page = nullptr;
    fz_pixmap* pixmap = nullptr;
    fz_device* device = nullptr;

    fz_try(ctx) {
        fz_matrix ctm = fz_scale(zoom, zoom);

        page = fz_load_page(ctx, doc, page_index);

        fz_rect bounds = fz_bound_page(ctx, page);
        fz_irect bbox = fz_round_rect(fz_transform_rect(bounds, ctm));

        pixmap = fz_new_pixmap_with_bbox(ctx, fz_device_rgb(ctx), bbox, nullptr, 0);
        fz_clear_pixmap_with_value(ctx, pixmap, 0xFF);

        device = fz_new_draw_device(ctx, ctm, pixmap);
        fz_run_page(ctx, page, device, fz_identity, nullptr);
        fz_close_device(ctx, device);

        const int width = fz_pixmap_width(ctx, pixmap);
        const int height = fz_pixmap_height(ctx, pixmap);
        const int stride = fz_pixmap_stride(ctx, pixmap);
        const int components = fz_pixmap_components(ctx, pixmap);
        const unsigned char* samples = fz_pixmap_samples(ctx, pixmap);

        out_page.width = width;
        out_page.height = height;
        out_page.pixels_rgba.resize(static_cast<size_t>(width) * static_cast<size_t>(height) * 4);

        for (int y = 0; y < height; ++y) {
            const unsigned char* src_row = samples + y * stride;
            std::uint8_t* dst_row =
                out_page.pixels_rgba.data() + static_cast<size_t>(y) * static_cast<size_t>(width) * 4;

            for (int x = 0; x < width; ++x) {
                const unsigned char* src = src_row + x * components;
                std::uint8_t* dst = dst_row + x * 4;

                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = 255;
            }
        }
    }
    fz_always(ctx) {
        if (device) {
            fz_drop_device(ctx, device);
        }
        if (pixmap) {
            fz_drop_pixmap(ctx, pixmap);
        }
        if (page) {
            fz_drop_page(ctx, page);
        }
    }
    fz_catch(ctx) {
        return false;
    }

    return true;
}

}  // namespace no::pdf
