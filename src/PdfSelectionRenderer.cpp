#include "pdf/PdfSelectionRenderer.hpp"

#include <mupdf/fitz.h>

#include <algorithm>
#include <cstring>

#include "pdf/PdfDocument.hpp"

namespace no::pdf {

void PdfSelectionRenderer::GetTargetSize(ExportPreset preset, int& out_w, int& out_h) const {
    if (preset == ExportPreset::InkPad4Landscape) {
        out_w = 1872;
        out_h = 1404;
    } else {
        out_w = 1404;
        out_h = 1872;
    }
}

bool PdfSelectionRenderer::RenderSelection(const PdfDocument& document, const PdfSelection& selection,
                                           ExportPreset preset, image::GrayImage& out_image) const {
    int target_w = 0;
    int target_h = 0;
    GetTargetSize(preset, target_w, target_h);

    out_image.Width = target_w;
    out_image.Height = target_h;
    out_image.Pixels.resize(target_w * target_h, 255);  // white background

    fz_context* ctx = document.GetContext();
    fz_document* doc = document.GetHandle();

    fz_page* page = nullptr;
    fz_device* dev = nullptr;
    fz_pixmap* pix = nullptr;

    fz_try(ctx) {
        page = fz_load_page(ctx, doc, selection.PageIndex);

        const fz_rect bounds = fz_bound_page(ctx, page);

        const float page_w = bounds.x1 - bounds.x0;
        const float page_h = bounds.y1 - bounds.y0;

        // Selection normalized
        const float sx0 = selection.X / page_w;
        const float sy0 = selection.Y / page_h;
        const float sx1 = (selection.X + selection.Width) / page_w;
        const float sy1 = (selection.Y + selection.Height) / page_h;

        // Absolute selection in page coords
        const float sel_x0 = bounds.x0 + sx0 * page_w;
        const float sel_y0 = bounds.y0 + sy0 * page_h;
        const float sel_x1 = bounds.x0 + sx1 * page_w;
        const float sel_y1 = bounds.y0 + sy1 * page_h;

        const float sel_w = sel_x1 - sel_x0;
        const float sel_h = sel_y1 - sel_y0;

        // scale to fit into target while preserving aspect
        const float scale_x = static_cast<float>(target_w) / sel_w;
        const float scale_y = static_cast<float>(target_h) / sel_h;
        const float scale = std::min(scale_x, scale_y);

        const float render_w = sel_w * scale;
        const float render_h = sel_h * scale;

        const float offset_x = (target_w - render_w) * 0.5f;
        const float offset_y = (target_h - render_h) * 0.5f;

        fz_matrix m = fz_scale(scale, scale);

        // translate so selection top-left maps to (offset_x, offset_y)
        m = fz_pre_translate(m, -sel_x0, -sel_y0);
        m = fz_pre_translate(m, offset_x, offset_y);

        fz_irect bbox = fz_make_irect(0, 0, target_w, target_h);

        pix = fz_new_pixmap_with_bbox(ctx, fz_device_gray(ctx), bbox, nullptr, 1);
        fz_clear_pixmap_with_value(ctx, pix, 255);

        dev = fz_new_draw_device(ctx, m, pix);
        fz_run_page(ctx, page, dev, fz_identity, nullptr);

        unsigned char* src = fz_pixmap_samples(ctx, pix);
        const int stride = fz_pixmap_stride(ctx, pix);

        for (int y = 0; y < target_h; ++y) {
            std::memcpy(&out_image.Pixels[y * target_w], src + y * stride, target_w);
        }
    }
    fz_always(ctx) {
        if (dev) fz_drop_device(ctx, dev);
        if (pix) fz_drop_pixmap(ctx, pix);
        if (page) fz_drop_page(ctx, page);
    }
    fz_catch(ctx) {
        return false;
    }

    return true;
}

}  // namespace no::pdf
