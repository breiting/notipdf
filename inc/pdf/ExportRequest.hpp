#pragma once

#include <filesystem>
#include <string>

#include "pdf/PdfSelection.hpp"

namespace no::pdf {

/**
 * \brief Describes a single PDF export operation.
 */
struct ExportRequest {
    std::filesystem::path InputPdfPath;
    std::filesystem::path OutputPdfPath;
    PdfSelection Selection;

    int RenderedPageWidth = 0;
    int RenderedPageHeight = 0;

    bool OptimizeForEInk = false;
};

}  // namespace no::pdf
