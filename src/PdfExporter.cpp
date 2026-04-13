#include "pdf/PdfExporter.hpp"

#include <filesystem>
#include <iostream>

#include "core/Logger.hpp"

namespace no::pdf {

namespace {

ExportPreset GetRenderPresetForRotation(ExportPreset final_preset, int rotation_degrees) {
    const int normalized = ((rotation_degrees % 360) + 360) % 360;

    if (normalized == 90 || normalized == 270) {
        if (final_preset == ExportPreset::InkPad4Landscape) {
            return ExportPreset::InkPad4Portrait;
        }
        return ExportPreset::InkPad4Landscape;
    }

    return final_preset;
}

image::GrayImage RotateImageCCW90(const image::GrayImage& src) {
    image::GrayImage dst;
    dst.Width = src.Height;
    dst.Height = src.Width;
    dst.Pixels.assign(static_cast<size_t>(dst.Width) * static_cast<size_t>(dst.Height), 255);

    for (int y = 0; y < src.Height; ++y) {
        for (int x = 0; x < src.Width; ++x) {
            const int dst_x = y;
            const int dst_y = src.Width - 1 - x;
            dst.Pixels[static_cast<size_t>(dst_y) * static_cast<size_t>(dst.Width) + static_cast<size_t>(dst_x)] =
                src.Pixels[static_cast<size_t>(y) * static_cast<size_t>(src.Width) + static_cast<size_t>(x)];
        }
    }

    return dst;
}

image::GrayImage RotateImage180(const image::GrayImage& src) {
    image::GrayImage dst;
    dst.Width = src.Width;
    dst.Height = src.Height;
    dst.Pixels.assign(static_cast<size_t>(dst.Width) * static_cast<size_t>(dst.Height), 255);

    for (int y = 0; y < src.Height; ++y) {
        for (int x = 0; x < src.Width; ++x) {
            const int dst_x = src.Width - 1 - x;
            const int dst_y = src.Height - 1 - y;
            dst.Pixels[static_cast<size_t>(dst_y) * static_cast<size_t>(dst.Width) + static_cast<size_t>(dst_x)] =
                src.Pixels[static_cast<size_t>(y) * static_cast<size_t>(src.Width) + static_cast<size_t>(x)];
        }
    }

    return dst;
}

image::GrayImage RotateImage270(const image::GrayImage& src) {
    image::GrayImage dst;
    dst.Width = src.Height;
    dst.Height = src.Width;
    dst.Pixels.assign(static_cast<size_t>(dst.Width) * static_cast<size_t>(dst.Height), 255);

    for (int y = 0; y < src.Height; ++y) {
        for (int x = 0; x < src.Width; ++x) {
            const int dst_x = src.Height - 1 - y;
            const int dst_y = x;
            dst.Pixels[static_cast<size_t>(dst_y) * static_cast<size_t>(dst.Width) + static_cast<size_t>(dst_x)] =
                src.Pixels[static_cast<size_t>(y) * static_cast<size_t>(src.Width) + static_cast<size_t>(x)];
        }
    }

    return dst;
}

image::GrayImage ApplyRotation(const image::GrayImage& src, int rotation_degrees) {
    const int normalized = ((rotation_degrees % 360) + 360) % 360;

    if (normalized == 90) {
        return RotateImageCCW90(src);
    }

    if (normalized == 180) {
        return RotateImage180(src);
    }

    if (normalized == 270) {
        return RotateImage270(src);
    }

    return src;
}

}  // namespace

bool PdfExporter::Export(const PdfDocument& document, const PdfSelection& selection,
                         const std::filesystem::path& output_path, ExportPreset preset, int rotation_degrees,
                         bool enable_optimization, const no::image::ImageOptimizationSettings& optimization_settings) {
    image::GrayImage image;

    const ExportPreset render_preset = GetRenderPresetForRotation(preset, rotation_degrees);

    if (!m_Renderer.RenderSelection(document, selection, render_preset, 0, image)) {
        LOG(Error) << "PdfExporter: RenderSelection failed.\n";
        return false;
    }

    image = ApplyRotation(image, rotation_degrees);

    if (enable_optimization) {
        m_ImageOptimizer.Optimize(optimization_settings, image);
    }

    const std::filesystem::path pgm_path = output_path.parent_path() / (output_path.stem().string() + ".pgm");

    // if (!m_Writer.WritePgm(image, pgm_path)) {
    //     LOG(Error) << "PdfExporter: WritePgm failed.\n";
    //     return false;
    // }
    // LOG(Info) << "Debug PGM written to: " << pgm_path;

    if (!m_Writer.Write(image, output_path, m_Backend)) {
        LOG(Error) << "PdfExporter: PDF write failed.\n";
        return false;
    }

    return true;
}

}  // namespace no::pdf
