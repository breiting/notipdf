#pragma once

#include "pdf/ExportRequest.hpp"
#include "pdf/ExportResult.hpp"

namespace no::pdf {

class PdfDocument;
}

namespace no::core {
class ProcessRunner;
}

namespace no::pdf {

/**
 * \brief Exports selected page regions to standalone PDF files.
 *
 * The current MVP implementation uses official MuPDF command line tools
 * to isolate a page and trim it to the selected rectangle.
 */
class PdfExporter {
   public:
    /**
     * \brief Constructs an exporter with an external process runner.
     *
     * \param process_runner The process execution dependency.
     */
    explicit PdfExporter(const no::core::ProcessRunner& process_runner);

    /**
     * \brief Exports a selected region from the input document.
     *
     * \param document The currently opened source document.
     * \param request The export request.
     * \return The export result.
     */
    ExportResult ExportSelection(const PdfDocument& document, const ExportRequest& request) const;

   private:
    struct PageBounds {
        double X0 = 0.0;
        double Y0 = 0.0;
        double X1 = 0.0;
        double Y1 = 0.0;
    };

    bool GetPageBounds(const PdfDocument& document, int page_index, PageBounds& out_bounds) const;

    ExportResult ExtractSinglePage(const ExportRequest& request, const std::string& temp_single_page_path) const;

    ExportResult TrimSelection(const ExportRequest& request, const PageBounds& page_bounds,
                               const std::string& temp_single_page_path) const;

    const no::core::ProcessRunner& m_ProcessRunner;
};

}  // namespace no::pdf
