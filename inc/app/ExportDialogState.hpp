
#pragma once

#include <string>

#include "app/LibraryCatalog.hpp"

namespace no::app {

/**
 * \brief Holds the transient UI state for the export dialog.
 */
struct ExportDialogState {
    bool IsOpen = false;

    BookId Book = BookId::Marschbuch;
    int PieceNumber = 1;
    std::string Title;

    PartId Part = PartId::Trompete1;

    bool OptimizeForEInk = true;
    bool ThresholdToBlackAndWhite = false;
};

}  // namespace no::app
