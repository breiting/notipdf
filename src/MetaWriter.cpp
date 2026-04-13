#include <fstream>
#include <nlohmann/json.hpp>

#include "pdf/MetaWriter.hpp"

namespace no::pdf {

bool MetaWriter::Write(const MetaData& meta_data, const std::filesystem::path& output_path) const {
    nlohmann::json json;
    json["title"] = meta_data.Title;
    json["author"] = meta_data.Author;
    json["voice"] = meta_data.Voice;
    json["pdf"] = meta_data.PdfFileName;

    std::ofstream file(output_path);
    if (!file.is_open()) {
        return false;
    }

    file << json.dump(4);
    return file.good();
}

}  // namespace no::pdf
