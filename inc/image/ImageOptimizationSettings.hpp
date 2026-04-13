#pragma once

namespace no::image {

/**
 * \brief Defines optional image optimization steps for E-Ink output.
 */
struct ImageOptimizationSettings {
    bool EnableContrastStretch = true;
    bool EnableWhiteClip = true;
    bool EnableSharpen = true;
    bool EnableThreshold = false;

    float SharpenAmount = 0.35f;

    int WhiteClipThreshold = 235;
    int ThresholdValue = 180;
};

}  // namespace no::image
