#pragma once

#include <filesystem>

#include "app/AppConfig.hpp"

namespace no::app {

/**
 * \brief Loads and stores application configuration.
 */
class ConfigService {
   public:
    /**
     * \brief Loads configuration from disk or returns defaults.
     *
     * \param output_config The loaded configuration.
     * \return True if a config file was loaded, false if defaults are used.
     */
    bool Load(AppConfig& output_config) const;

    /**
     * \brief Saves configuration to disk.
     *
     * \param config The configuration to save.
     * \return True on success.
     */
    bool Save(const AppConfig& config) const;

    /**
     * \brief Returns the config file path.
     */
    std::filesystem::path GetConfigPath() const;

   private:
    std::filesystem::path GetConfigDirectory() const;
};

}  // namespace no::app
