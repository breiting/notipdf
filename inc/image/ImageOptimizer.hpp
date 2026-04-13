#pragma once

#include "image/GrayImage.hpp"
#include "image/ImageOptimizationSettings.hpp"

namespace no::image {

/**
 * \brief Applies lightweight grayscale optimizations for E-Ink displays.
 */
class ImageOptimizer {
   public:
    /**
     * \brief Optimizes an image in place.
     *
     * \param settings The optimization settings.
     * \param image The image to modify.
     */
    void Optimize(const ImageOptimizationSettings& settings, GrayImage& image) const;

   private:
    void ApplyWhiteClip(int threshold, GrayImage& image) const;
    void ApplyContrastStretch(GrayImage& image) const;
    void ApplySharpen(float amount, GrayImage& image) const;
    void ApplyThreshold(int threshold, GrayImage& image) const;
};

}  // namespace no::image
