#pragma once

#include <filesystem>

#include "app/LibraryCatalog.hpp"
#include "pdf/PdfBackend.hpp"

namespace no::app {

/**
 * \brief Stores persisted application configuration.
 */
struct AppConfig {
    std::filesystem::path OutputDirectory = std::filesystem::current_path();

    bool DefaultOptimizeForEInk = true;
    bool DefaultThresholdBlackWhite = false;

    BookId DefaultBook = BookId::Marschbuch;
    InstrumentId DefaultInstrument = InstrumentId::Trompete1;

    pdf::PdfBackend Backend = pdf::PdfBackend::Sips;
};

}  // namespace no::app
