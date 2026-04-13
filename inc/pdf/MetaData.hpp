#pragma once

#include <string>

namespace no::pdf {

/**
 * \brief Describes piece-level metadata written alongside exported PDFs.
 */
struct MetaData {
    std::string BookId;
    std::string BookTitle;

    int PieceNumber = 0;
    std::string PieceId;
    std::string PieceTitle;

    std::string PartId;
    std::string PartTitle;
    std::string PdfFileName;
};

}  // namespace no::pdf
