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
    /**
     * \brief Writes a grayscale image as a raw PGM file for debugging.
     *
     * \param image The source image.
     * \param output_path The destination path.
     * \return True on success, false otherwise.
     */
    bool WritePgm(const image::GrayImage& image, const std::filesystem::path& output_path) const;
};

}  // namespace no::pdf
