#pragma once

#include <filesystem>
#include <string>

#include "image/ImageOptimizationSettings.hpp"
#include "pdf/ExportPreset.hpp"
#include "pdf/PdfSelection.hpp"

namespace no::pdf {

/**
 * \brief Describes a single selection export request.
 */
struct ExportRequest {
    std::filesystem::path InputPdfPath;
    std::filesystem::path OutputPdfPath;
    PdfSelection Selection;

    ExportPreset Preset = ExportPreset::InkPad4Portrait;
    bool EnableOptimization = true;
    image::ImageOptimizationSettings OptimizationSettings;
};

}  // namespace no::pdf
