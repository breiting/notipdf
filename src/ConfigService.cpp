#include "app/ConfigService.hpp"

#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>

namespace no::app {

std::filesystem::path ConfigService::GetConfigDirectory() const {
    const char* home = std::getenv("HOME");
    if (home == nullptr) {
        return std::filesystem::current_path() / ".notipdf";
    }

    return std::filesystem::path(home) / ".notipdf";
}

std::filesystem::path ConfigService::GetConfigPath() const {
    return GetConfigDirectory() / "config.json";
}

bool ConfigService::Load(AppConfig& output_config) const {
    const std::filesystem::path path = GetConfigPath();

    if (!std::filesystem::exists(path)) {
        return false;
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json json;
    file >> json;

    if (json.contains("output_directory")) {
        output_config.OutputDirectory = json["output_directory"].get<std::string>();
    }

    if (json.contains("default_optimize_for_eink")) {
        output_config.DefaultOptimizeForEInk = json["default_optimize_for_eink"].get<bool>();
    }

    if (json.contains("default_threshold_black_white")) {
        output_config.DefaultThresholdBlackWhite = json["default_threshold_black_white"].get<bool>();
    }

    if (json.contains("default_voice")) {
        output_config.DefaultVoiceIndex = json["default_voice"].get<int>();
    }

    return true;
}

bool ConfigService::Save(const AppConfig& config) const {
    const std::filesystem::path directory = GetConfigDirectory();
    std::error_code ec;
    std::filesystem::create_directories(directory, ec);

    std::ofstream file(GetConfigPath());
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json json;
    json["output_directory"] = config.OutputDirectory.string();
    json["default_optimize_for_eink"] = config.DefaultOptimizeForEInk;
    json["default_threshold_black_white"] = config.DefaultThresholdBlackWhite;
    json["default_voice"] = config.DefaultVoiceIndex;

    file << json.dump(4);
    return file.good();
}

}  // namespace no::app
