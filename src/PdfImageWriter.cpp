#include "pdf/PdfImageWriter.hpp"

#include "core/Logger.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <spawn.h>
#include <stb_image_write.h>
#include <sys/wait.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#include "pdf/PdfBackend.hpp"

extern char** environ;

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

bool RunConvertDirect(const std::string& input, const std::string& output) {
    pid_t pid = 0;

    char* const argv[] = {const_cast<char*>("/usr/bin/convert"), const_cast<char*>(input.c_str()),
                          const_cast<char*>(output.c_str()), nullptr};

    const int spawn_result = posix_spawn(&pid, "/usr/bin/convert", nullptr, nullptr, argv, environ);

    if (spawn_result != 0) {
        LOG(Error) << "posix_spawn failed: " << spawn_result << '\n';
        return false;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        LOG(Error) << "waitpid failed.\n";
        return false;
    }

    if (WIFEXITED(status)) {
        const int exit_code = WEXITSTATUS(status);
        std::cout << "convert exit code: " << exit_code << '\n';
        return exit_code == 0;
    }

    if (WIFSIGNALED(status)) {
        LOG(Error) << "convert terminated by signal: " << WTERMSIG(status) << '\n';
    }

    return false;
}

}  // namespace

bool PdfImageWriter::Write(const image::GrayImage& image, const std::filesystem::path& output_path,
                           pdf::PdfBackend backend) const {
    if (image.Width <= 0 || image.Height <= 0 || image.Pixels.empty()) {
        return false;
    }

    const std::filesystem::path temp_png_path = output_path.parent_path() / (output_path.stem().string() + ".tmp.png");

    if (!WritePng(image, temp_png_path)) {
        return false;
    }

    if (backend == PdfBackend::Magick) {
        return RunConvertDirect(temp_png_path.string(), output_path.string());
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
