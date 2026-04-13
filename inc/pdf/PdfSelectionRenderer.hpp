#pragma once

#include "image/GrayImage.hpp"
#include "pdf/ExportPreset.hpp"
#include "pdf/PdfSelection.hpp"

namespace no::pdf {

class PdfDocument;

/**
 * \brief Renders a selected PDF region into a fixed-resolution grayscale image.
 */
class PdfSelectionRenderer {
   public:
    /**
     * \brief Renders a selection into a target preset resolution.
     *
     * \param document The source PDF document.
     * \param selection The selected region in page coordinates.
     * \param preset The export preset.
     * \param out_image Output grayscale image.
     * \return True on success.
     */
    bool RenderSelection(const PdfDocument& document, const PdfSelection& selection, ExportPreset preset,
                         image::GrayImage& out_image) const;

   private:
    void GetTargetSize(ExportPreset preset, int& out_w, int& out_h) const;
};

}  // namespace no::pdf
