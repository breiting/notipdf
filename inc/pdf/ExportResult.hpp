#pragma once

#include <string>

namespace no::pdf {

/**
 * \brief Contains the result of a PDF export operation.
 */
struct ExportResult {
    bool Success = false;
    std::string Message;
};

}  // namespace no::pdf
