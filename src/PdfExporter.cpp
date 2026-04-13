#include "pdf/PdfExporter.hpp"

#include <mupdf/fitz.h>

#include <filesystem>
#include <sstream>

#include "core/ProcessRunner.hpp"
#include "pdf/PdfDocument.hpp"

namespace no::pdf {

PdfExporter::PdfExporter(const no::core::ProcessRunner& process_runner) : m_ProcessRunner(process_runner) {
}

ExportResult PdfExporter::ExportSelection(const PdfDocument& document, const ExportRequest& request) const {
    if (!document.IsOpen()) {
        return {false, "Document is not open."};
    }

    if (request.Selection.Width <= 0.0f || request.Selection.Height <= 0.0f) {
        return {false, "Selection is empty."};
    }

    if (request.RenderedPageWidth <= 0 || request.RenderedPageHeight <= 0) {
        return {false, "Rendered page size is invalid."};
    }

    PageBounds page_bounds;
    if (!GetPageBounds(document, request.Selection.PageIndex, page_bounds)) {
        return {false, "Failed to read page bounds from MuPDF."};
    }

    const std::filesystem::path output_path = request.OutputPdfPath;
    const std::filesystem::path temp_single_page_path =
        output_path.parent_path() / (output_path.stem().string() + ".singlepage.tmp.pdf");

    const ExportResult extract_result = ExtractSinglePage(request, temp_single_page_path.string());
    if (!extract_result.Success) {
        return extract_result;
    }

    const ExportResult trim_result = TrimSelection(request, page_bounds, temp_single_page_path.string());

    std::error_code ec;
    std::filesystem::remove(temp_single_page_path, ec);

    return trim_result;
}

bool PdfExporter::GetPageBounds(const PdfDocument& document, int page_index, PageBounds& out_bounds) const {
    fz_context* ctx = document.GetContext();
    fz_document* doc = document.GetHandle();

    fz_page* page = nullptr;

    fz_try(ctx) {
        page = fz_load_page(ctx, doc, page_index);
        const fz_rect bounds = fz_bound_page(ctx, page);

        out_bounds.X0 = bounds.x0;
        out_bounds.Y0 = bounds.y0;
        out_bounds.X1 = bounds.x1;
        out_bounds.Y1 = bounds.y1;
    }
    fz_always(ctx) {
        if (page != nullptr) {
            fz_drop_page(ctx, page);
        }
    }
    fz_catch(ctx) {
        return false;
    }

    return true;
}

ExportResult PdfExporter::ExtractSinglePage(const ExportRequest& request,
                                            const std::string& temp_single_page_path) const {
    const int one_based_page = request.Selection.PageIndex + 1;

    const no::core::ProcessResult result = m_ProcessRunner.Run(
        "mutool", {"clean", request.InputPdfPath.string(), temp_single_page_path, std::to_string(one_based_page)});

    if (result.ExitCode != 0) {
        return {false, "mutool clean failed: " + result.CommandLine};
    }

    return {true, {}};
}

ExportResult PdfExporter::TrimSelection(const ExportRequest& request, const PageBounds& page_bounds,
                                        const std::string& temp_single_page_path) const {
    const double page_width_points = page_bounds.X1 - page_bounds.X0;
    const double page_height_points = page_bounds.Y1 - page_bounds.Y0;

    const double left =
        (static_cast<double>(request.Selection.X) / static_cast<double>(request.RenderedPageWidth)) * page_width_points;

    const double top = (static_cast<double>(request.Selection.Y) / static_cast<double>(request.RenderedPageHeight)) *
                       page_height_points;

    const double right = page_width_points - ((static_cast<double>(request.Selection.X + request.Selection.Width) /
                                               static_cast<double>(request.RenderedPageWidth)) *
                                              page_width_points);

    const double bottom = page_height_points - ((static_cast<double>(request.Selection.Y + request.Selection.Height) /
                                                 static_cast<double>(request.RenderedPageHeight)) *
                                                page_height_points);

    std::ostringstream margins;
    margins << top << "," << right << "," << bottom << "," << left;

    const no::core::ProcessResult result = m_ProcessRunner.Run(
        "mutool",
        {"trim", "-b", "mediabox", "-m", margins.str(), "-o", request.OutputPdfPath.string(), temp_single_page_path});

    if (result.ExitCode != 0) {
        return {false, "mutool trim failed: " + result.CommandLine};
    }

    return {true, {}};
}

}  // namespace no::pdf
