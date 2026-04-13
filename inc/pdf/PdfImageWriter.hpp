#pragma once

#include <filesystem>

#include "image/GrayImage.hpp"

namespace no::pdf {

/**
 * \brief Writes a grayscale image into a single-page PDF.
 */
class PdfImageWriter {
   public:
    bool Write(const image::GrayImage& image, const std::filesystem::path& output_path) const;
};

}  // namespace no::pdf
