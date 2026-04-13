#pragma once

#include "pdf/ExportPreset.hpp"
#include "pdf/PdfImageWriter.hpp"
#include "pdf/PdfSelectionRenderer.hpp"

namespace no::pdf {

class PdfDocument;

/**
 * \brief High-level export pipeline.
 */
class PdfExporter {
   public:
    bool Export(const PdfDocument& document, const PdfSelection& selection, const std::filesystem::path& output_path,
                ExportPreset preset);

   private:
    PdfSelectionRenderer m_Renderer;
    PdfImageWriter m_Writer;
};

}  // namespace no::pdf
