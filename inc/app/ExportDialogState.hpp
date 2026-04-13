#pragma once

#include <string>

namespace no::app {

/**
 * \brief Holds the transient UI state for the export dialog.
 */
struct ExportDialogState {
    bool IsOpen = false;

    std::string Title;
    std::string Author;

    int VoiceIndex = 0;
};

}  // namespace no::app
