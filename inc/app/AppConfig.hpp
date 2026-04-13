#pragma once

#include <filesystem>

namespace no::app {

/**
 * \brief Stores persisted application configuration.
 */
struct AppConfig {
    std::filesystem::path OutputDirectory = std::filesystem::current_path();

    bool DefaultOptimizeForEInk = true;
    bool DefaultThresholdBlackWhite = false;

    int DefaultVoiceIndex = 0;
};

}  // namespace no::app
