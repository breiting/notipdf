#include "pdf/PdfExporter.hpp"

#include <filesystem>
#include <iostream>

namespace no::pdf {

bool PdfExporter::Export(const PdfDocument& document, const PdfSelection& selection,
                         const std::filesystem::path& output_path, ExportPreset preset, bool enable_optimization,
                         const no::image::ImageOptimizationSettings& optimization_settings) {
    image::GrayImage image;

    if (!m_Renderer.RenderSelection(document, selection, preset, image)) {
        std::cerr << "PdfExporter: RenderSelection failed.\n";
        return false;
    }

    if (enable_optimization) {
        m_ImageOptimizer.Optimize(optimization_settings, image);
    }

    const std::filesystem::path pgm_path = output_path.parent_path() / (output_path.stem().string() + ".pgm");

    // if (!m_Writer.WritePgm(image, pgm_path)) {
    //     std::cerr << "PdfExporter: WritePgm failed.\n";
    //     return false;
    // }
    // std::cout << "Debug PGM written to: " << pgm_path << '\n';

    if (!m_Writer.Write(image, output_path)) {
        std::cerr << "PdfExporter: PDF write failed.\n";
        return false;
    }

    return true;
}

}  // namespace no::pdf
