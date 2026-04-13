#pragma once

#include <filesystem>

#include "pdf/MetaData.hpp"

namespace no::pdf {

/**
 * \brief Writes metadata files for exported PDF selections.
 */
class MetaWriter {
   public:
    /**
     * \brief Writes a meta.json file to disk.
     *
     * \param meta_data The metadata to write.
     * \param output_path The destination json path.
     * \return True on success, false otherwise.
     */
    bool Write(const MetaData& meta_data, const std::filesystem::path& output_path) const;
};

}  // namespace no::pdf
