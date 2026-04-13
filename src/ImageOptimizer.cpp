#include "image/ImageOptimizer.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace no::image {

void ImageOptimizer::Optimize(const ImageOptimizationSettings& settings, GrayImage& image) const {
    if (image.Width <= 0 || image.Height <= 0 || image.Pixels.empty()) {
        return;
    }

    if (settings.EnableWhiteClip) {
        ApplyWhiteClip(settings.WhiteClipThreshold, image);
    }

    if (settings.EnableContrastStretch) {
        ApplyContrastStretch(image);
    }

    if (settings.EnableSharpen) {
        ApplySharpen(settings.SharpenAmount, image);
    }

    if (settings.EnableThreshold) {
        ApplyThreshold(settings.ThresholdValue, image);
    }
}

void ImageOptimizer::ApplyWhiteClip(int threshold, GrayImage& image) const {
    const std::uint8_t white_clip = static_cast<std::uint8_t>(std::clamp(threshold, 0, 255));

    for (std::uint8_t& pixel : image.Pixels) {
        if (pixel >= white_clip) {
            pixel = 255;
        }
    }
}

void ImageOptimizer::ApplyContrastStretch(GrayImage& image) const {
    std::uint8_t min_value = 255;
    std::uint8_t max_value = 0;

    for (std::uint8_t pixel : image.Pixels) {
        if (pixel < min_value) {
            min_value = pixel;
        }
        if (pixel > max_value) {
            max_value = pixel;
        }
    }

    if (max_value <= min_value) {
        return;
    }

    const float scale = 255.0f / static_cast<float>(max_value - min_value);

    for (std::uint8_t& pixel : image.Pixels) {
        const int value = static_cast<int>(std::lround((static_cast<int>(pixel) - min_value) * scale));
        pixel = static_cast<std::uint8_t>(std::clamp(value, 0, 255));
    }
}

void ImageOptimizer::ApplySharpen(float amount, GrayImage& image) const {
    if (image.Width < 3 || image.Height < 3 || amount <= 0.0f) {
        return;
    }

    std::vector<std::uint8_t> original = image.Pixels;

    const float clamped_amount = std::clamp(amount, 0.0f, 2.0f);

    auto sample = [&](int x, int y) -> int {
        x = std::clamp(x, 0, image.Width - 1);
        y = std::clamp(y, 0, image.Height - 1);
        return original[y * image.Width + x];
    };

    for (int y = 0; y < image.Height; ++y) {
        for (int x = 0; x < image.Width; ++x) {
            const int center = sample(x, y);
            const int left = sample(x - 1, y);
            const int right = sample(x + 1, y);
            const int up = sample(x, y - 1);
            const int down = sample(x, y + 1);

            const float laplacian = 4.0f * static_cast<float>(center) - static_cast<float>(left) -
                                    static_cast<float>(right) - static_cast<float>(up) - static_cast<float>(down);

            const int value = static_cast<int>(std::lround(center + clamped_amount * laplacian));
            image.Pixels[y * image.Width + x] = static_cast<std::uint8_t>(std::clamp(value, 0, 255));
        }
    }
}

void ImageOptimizer::ApplyThreshold(int threshold, GrayImage& image) const {
    const std::uint8_t t = static_cast<std::uint8_t>(std::clamp(threshold, 0, 255));

    for (std::uint8_t& pixel : image.Pixels) {
        pixel = (pixel >= t) ? 255 : 0;
    }
}

}  // namespace no::image
