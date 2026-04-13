#include "pdf/PdfSelectionRenderer.hpp"

#include <mupdf/fitz.h>

#include <algorithm>
#include <cstring>
#include <iostream>

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
    out_image.Pixels.assign(target_w * target_h, 255);

    fz_context* ctx = document.GetContext();
    fz_document* doc = document.GetHandle();

    fz_page* page = nullptr;
    fz_device* dev = nullptr;
    fz_pixmap* pix = nullptr;

    fz_try(ctx) {
        page = fz_load_page(ctx, doc, selection.PageIndex);

        const fz_rect bounds = fz_bound_page(ctx, page);

        const float sel_x0 = bounds.x0;
        const float sel_y0 = bounds.y0;
        const float sel_x1 = bounds.x1;
        const float sel_y1 = bounds.y1;

        // const float sel_x0 = selection.X;
        // const float sel_y0 = selection.Y;
        // const float sel_x1 = selection.X + selection.Width;
        // const float sel_y1 = selection.Y + selection.Height;

        const float sel_w = sel_x1 - sel_x0;
        const float sel_h = sel_y1 - sel_y0;

        if (sel_w <= 0.0f || sel_h <= 0.0f) {
            fz_throw(ctx, FZ_ERROR_ARGUMENT, "Invalid selection size.");
        }

        const float scale_x = static_cast<float>(target_w) / sel_w;
        const float scale_y = static_cast<float>(target_h) / sel_h;
        const float scale = std::min(scale_x, scale_y);

        const float render_w = sel_w * scale;
        const float render_h = sel_h * scale;

        const float offset_x = (static_cast<float>(target_w) - render_w) * 0.5f;
        const float offset_y = (static_cast<float>(target_h) - render_h) * 0.5f;

        fz_matrix m;
        m.a = scale;
        m.b = 0.0f;
        m.c = 0.0f;
        m.d = scale;
        m.e = offset_x - sel_x0 * scale;
        m.f = offset_y - sel_y0 * scale;

        const fz_irect bbox = fz_make_irect(0, 0, target_w, target_h);

        // IMPORTANT: alpha = 0, so we get exactly 1 byte per pixel in Gray mode.
        pix = fz_new_pixmap_with_bbox(ctx, fz_device_gray(ctx), bbox, nullptr, 0);
        fz_clear_pixmap_with_value(ctx, pix, 255);

        dev = fz_new_draw_device(ctx, m, pix);
        fz_run_page(ctx, page, dev, fz_identity, nullptr);
        fz_close_device(ctx, dev);

        const unsigned char* src = fz_pixmap_samples(ctx, pix);
        const int stride = fz_pixmap_stride(ctx, pix);
        const int width = fz_pixmap_width(ctx, pix);
        const int height = fz_pixmap_height(ctx, pix);
        const int components = fz_pixmap_components(ctx, pix);

        std::cout << "Selection page=" << selection.PageIndex << " x=" << selection.X << " y=" << selection.Y
                  << " w=" << selection.Width << " h=" << selection.Height << " | target=" << target_w << "x"
                  << target_h << " | pix=" << width << "x" << height << " | stride=" << stride
                  << " | components=" << components << " | scale=" << scale << " | offset=(" << offset_x << ","
                  << offset_y << ")"
                  << " | matrix=(" << m.a << "," << m.b << "," << m.c << "," << m.d << "," << m.e << "," << m.f
                  << ")\n";

        for (int y = 0; y < target_h; ++y) {
            std::memcpy(&out_image.Pixels[y * target_w], src + y * stride, target_w);
        }
    }
    fz_always(ctx) {
        if (dev != nullptr) {
            fz_drop_device(ctx, dev);
        }
        if (pix != nullptr) {
            fz_drop_pixmap(ctx, pix);
        }
        if (page != nullptr) {
            fz_drop_page(ctx, page);
        }
    }
    fz_catch(ctx) {
        return false;
    }

    return true;
}

}  // namespace no::pdf
