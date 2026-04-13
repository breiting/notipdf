#pragma once

#include <filesystem>

#include "image/GrayImage.hpp"
#include "pdf/PdfBackend.hpp"

namespace no::pdf {

/**
 * \brief Writes grayscale raster output to disk.
 *
 * The current MVP implementation writes a temporary PNG and converts it
 * to PDF via the macOS `sips` tool.
 */
class PdfImageWriter {
   public:
    /**
     * \brief Writes a grayscale image as a single-page PDF.
     *
     * \param image The source grayscale image.
     * \param output_path The destination PDF path.
     * \return True on success, false otherwise.
     */
    bool Write(const image::GrayImage& image, const std::filesystem::path& output_path,
               PdfBackend backend = PdfBackend::Sips) const;

    /**
     * \brief Writes a grayscale image as a raw PGM file for debugging.
     *
     * \param image The source image.
     * \param output_path The destination path.
     * \return True on success, false otherwise.
     */
    bool WritePgm(const image::GrayImage& image, const std::filesystem::path& output_path) const;

   private:
    /**
     * \brief Writes a grayscale image as a PNG file.
     *
     * \param image The source image.
     * \param output_path The destination PNG path.
     * \return True on success, false otherwise.
     */
    bool WritePng(const image::GrayImage& image, const std::filesystem::path& output_path) const;
};

}  // namespace no::pdf
