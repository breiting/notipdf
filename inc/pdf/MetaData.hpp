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
    std::string PieceAuthor;

    std::string InstrumentId;
    std::string InstrumentTitle;

    std::string PdfFileName;
};

}  // namespace no::pdf
