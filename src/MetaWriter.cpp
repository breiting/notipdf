#include "pdf/MetaWriter.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace no::pdf {

bool MetaWriter::WriteOrUpdate(const MetaData& meta_data, const std::filesystem::path& output_path) const {
    nlohmann::json json;

    if (std::filesystem::exists(output_path)) {
        std::ifstream input(output_path);
        if (input.is_open()) {
            input >> json;
        }
    }

    json["book"]["id"] = meta_data.BookId;
    json["book"]["title"] = meta_data.BookTitle;

    json["piece"]["number"] = meta_data.PieceNumber;
    json["piece"]["id"] = meta_data.PieceId;
    json["piece"]["title"] = meta_data.PieceTitle;

    if (!json.contains("parts") || !json["parts"].is_array()) {
        json["parts"] = nlohmann::json::array();
    }

    bool updated = false;
    for (auto& part : json["parts"]) {
        if (part.contains("id") && part["id"].get<std::string>() == meta_data.PartId) {
            part["id"] = meta_data.PartId;
            part["title"] = meta_data.PartTitle;
            part["pdf"] = meta_data.PdfFileName;
            updated = true;
            break;
        }
    }

    if (!updated) {
        nlohmann::json part;
        part["id"] = meta_data.PartId;
        part["title"] = meta_data.PartTitle;
        part["pdf"] = meta_data.PdfFileName;
        json["parts"].push_back(part);
    }

    std::ofstream output(output_path);
    if (!output.is_open()) {
        return false;
    }

    output << json.dump(4);
    return output.good();
}

}  // namespace no::pdf
