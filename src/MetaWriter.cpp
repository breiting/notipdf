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
    json["piece"]["author"] = meta_data.PieceAuthor;

    if (!json.contains("instruments") || !json["instruments"].is_array()) {
        json["instruments"] = nlohmann::json::array();
    }

    bool updated = false;
    for (auto& instrument : json["instruments"]) {
        if (instrument.contains("id") && instrument["id"].get<std::string>() == meta_data.InstrumentId) {
            instrument["id"] = meta_data.InstrumentId;
            instrument["title"] = meta_data.InstrumentTitle;
            instrument["pdf"] = meta_data.PdfFileName;
            updated = true;
            break;
        }
    }

    if (!updated) {
        nlohmann::json instrument;
        instrument["id"] = meta_data.InstrumentId;
        instrument["title"] = meta_data.InstrumentTitle;
        instrument["pdf"] = meta_data.PdfFileName;
        json["instruments"].push_back(instrument);
    }

    std::ofstream output(output_path);
    if (!output.is_open()) {
        return false;
    }

    output << json.dump(4);
    return output.good();
}

}  // namespace no::pdf
