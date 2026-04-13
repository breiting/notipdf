#include "pdf/PdfImageWriter.hpp"

#include <mupdf/fitz.h>

namespace no::pdf {

bool PdfImageWriter::Write(const image::GrayImage& image, const std::filesystem::path& output_path) const {
    fz_context* ctx = fz_new_context(nullptr, nullptr, FZ_STORE_DEFAULT);
    if (ctx == nullptr) {
        return false;
    }

    fz_document_writer* writer = nullptr;
    fz_device* dev = nullptr;
    fz_pixmap* pix = nullptr;
    fz_image* pdf_image = nullptr;

    fz_try(ctx) {
        writer = fz_new_document_writer(ctx, output_path.string().c_str(), "pdf", "");

        const float width_pt = static_cast<float>(image.Width) / 300.0f * 72.0f;
        const float height_pt = static_cast<float>(image.Height) / 300.0f * 72.0f;

        dev = fz_begin_page(ctx, writer, fz_make_rect(0.0f, 0.0f, width_pt, height_pt));

        pix = fz_new_pixmap_with_data(ctx, fz_device_gray(ctx), image.Width, image.Height, nullptr, 0, image.Width,
                                      const_cast<unsigned char*>(image.Pixels.data()));

        pdf_image = fz_new_image_from_pixmap(ctx, pix, nullptr);

        const fz_matrix m =
            fz_scale(width_pt / static_cast<float>(image.Width), height_pt / static_cast<float>(image.Height));

        fz_fill_image(ctx, dev, pdf_image, m, 1.0f, fz_default_color_params);

        fz_end_page(ctx, writer);
        fz_close_document_writer(ctx, writer);
    }
    fz_always(ctx) {
        if (pdf_image != nullptr) {
            fz_drop_image(ctx, pdf_image);
        }
        if (pix != nullptr) {
            fz_drop_pixmap(ctx, pix);
        }
        if (dev != nullptr) {
            fz_drop_device(ctx, dev);
        }
        if (writer != nullptr) {
            fz_drop_document_writer(ctx, writer);
        }
        fz_drop_context(ctx);
    }
    fz_catch(ctx) {
        return false;
    }

    return true;
}

}  // namespace no::pdf
