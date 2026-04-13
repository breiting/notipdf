#pragma once

#include <string>

namespace no::pdf {

/**
 * \brief Describes metadata written alongside an exported PDF.
 */
struct MetaData {
    std::string Title;
    std::string Author;
    std::string Voice;
    std::string PdfFileName;
};

}  // namespace no::pdf
