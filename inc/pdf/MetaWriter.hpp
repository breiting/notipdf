#pragma once

#include <filesystem>

#include "pdf/MetaData.hpp"

namespace no::pdf {

/**
 * \brief Writes and updates piece-level metadata files.
 */
class MetaWriter {
   public:
    /**
     * \brief Adds or updates one exported part inside a piece-level meta.json.
     *
     * \param meta_data The piece and part metadata.
     * \param output_path The destination meta.json path.
     * \return True on success, false otherwise.
     */
    bool WriteOrUpdate(const MetaData& meta_data, const std::filesystem::path& output_path) const;
};

}  // namespace no::pdf
