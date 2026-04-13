#include "pdf/PdfExporter.hpp"

namespace no::pdf {

bool PdfExporter::Export(const PdfDocument& document, const PdfSelection& selection,
                         const std::filesystem::path& output_path, ExportPreset preset) {
    image::GrayImage image;

    if (!m_Renderer.RenderSelection(document, selection, preset, image)) {
        return false;
    }

    return m_Writer.Write(image, output_path);
}

}  // namespace no::pdf
