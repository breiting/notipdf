#include "pdf/PdfImageWriter.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace no::pdf {

namespace {

/**
 * \brief Escapes a shell argument using single quotes.
 *
 * \param value The raw argument.
 * \return A shell-safe escaped string.
 */
std::string EscapeShellArgument(const std::string& value) {
    std::string escaped = "'";
    for (char c : value) {
        if (c == '\'') {
            escaped += "'\\''";
        } else {
            escaped += c;
        }
    }
    escaped += "'";
    return escaped;
}

}  // namespace

bool PdfImageWriter::Write(const image::GrayImage& image, const std::filesystem::path& output_path) const {
    if (image.Width <= 0 || image.Height <= 0 || image.Pixels.empty()) {
        return false;
    }

    const std::filesystem::path temp_png_path = output_path.parent_path() / (output_path.stem().string() + ".tmp.png");

    if (!WritePng(image, temp_png_path)) {
        return false;
    }

    const std::string command = "sips -s format pdf " + EscapeShellArgument(temp_png_path.string()) + " --out " +
                                EscapeShellArgument(output_path.string()) + " > /dev/null 2>&1";

    const int result = std::system(command.c_str());

    std::error_code ec;
    std::filesystem::remove(temp_png_path, ec);

    return result == 0;
}

bool PdfImageWriter::WritePgm(const image::GrayImage& image, const std::filesystem::path& output_path) const {
    if (image.Width <= 0 || image.Height <= 0 || image.Pixels.empty()) {
        return false;
    }

    std::ofstream file(output_path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file << "P5\n" << image.Width << " " << image.Height << "\n255\n";
    file.write(reinterpret_cast<const char*>(image.Pixels.data()), static_cast<std::streamsize>(image.Pixels.size()));

    return file.good();
}

bool PdfImageWriter::WritePng(const image::GrayImage& image, const std::filesystem::path& output_path) const {
    if (image.Width <= 0 || image.Height <= 0 || image.Pixels.empty()) {
        return false;
    }

    const int stride_bytes = image.Width;

    const int result =
        stbi_write_png(output_path.string().c_str(), image.Width, image.Height, 1, image.Pixels.data(), stride_bytes);

    return result != 0;
}

}  // namespace no::pdf
