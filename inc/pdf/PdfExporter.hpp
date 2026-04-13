#pragma once

#include <filesystem>

#include "image/ImageOptimizationSettings.hpp"
#include "image/ImageOptimizer.hpp"
#include "pdf/ExportPreset.hpp"
#include "pdf/PdfBackend.hpp"
#include "pdf/PdfImageWriter.hpp"
#include "pdf/PdfSelectionRenderer.hpp"

namespace no::pdf {

class PdfDocument;

/**
 * \brief High-level export pipeline.
 */
class PdfExporter {
   public:
    explicit PdfExporter(PdfBackend backend) : m_Backend(backend) {
    }

    bool Export(const PdfDocument& document, const PdfSelection& selection, const std::filesystem::path& output_path,
                ExportPreset preset, int rotation_degrees, bool enable_optimization,
                const no::image::ImageOptimizationSettings& optimization_settings);

   private:
    PdfBackend m_Backend = PdfBackend::Sips;
    PdfSelectionRenderer m_Renderer;
    PdfImageWriter m_Writer;
    no::image::ImageOptimizer m_ImageOptimizer;
};

}  // namespace no::pdf
