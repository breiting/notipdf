#include "app/ConfigService.hpp"

#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>

#include "pdf/PdfBackend.hpp"

namespace no::app {

std::filesystem::path ConfigService::GetConfigDirectory() const {
    const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME");
    if (xdg_config_home != nullptr && xdg_config_home[0] != '\0') {
        return std::filesystem::path(xdg_config_home) / "notipdf";
    }

    const char* home = std::getenv("HOME");
    if (home != nullptr && home[0] != '\0') {
        return std::filesystem::path(home) / ".config" / "notipdf";
    }

    return std::filesystem::current_path() / ".notipdf";
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

    if (json.contains("default_book")) {
        output_config.DefaultBook = static_cast<BookId>(json["default_book"].get<int>());
    }

    if (json.contains("default_part")) {
        output_config.DefaultPart = static_cast<PartId>(json["default_part"].get<int>());
    }

    if (json.contains("pdf_backend")) {
        const std::string backend = json["pdf_backend"].get<std::string>();

        if (backend == "magick") {
            output_config.Backend = pdf::PdfBackend::Magick;
        } else {
            output_config.Backend = pdf::PdfBackend::Sips;
        }
    }

    return true;
}

bool ConfigService::Save(const AppConfig& config) const {
    const std::filesystem::path directory = GetConfigDirectory();

    std::error_code ec;
    std::filesystem::create_directories(directory, ec);
    if (ec) {
        return false;
    }

    std::ofstream file(GetConfigPath());
    if (!file.is_open()) {
        return false;
    }

    nlohmann::json json;
    json["output_directory"] = config.OutputDirectory.string();
    json["default_optimize_for_eink"] = config.DefaultOptimizeForEInk;
    json["default_threshold_black_white"] = config.DefaultThresholdBlackWhite;
    json["default_book"] = static_cast<int>(config.DefaultBook);
    json["default_part"] = static_cast<int>(config.DefaultPart);
    json["pdf_backend"] = (config.Backend == no::pdf::PdfBackend::Magick) ? "magick" : "sips";

    file << json.dump(4);
    return file.good();
}

}  // namespace no::app
