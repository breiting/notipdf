#pragma once

#include <cstdint>
#include <vector>

namespace no::image {

/**
 * \brief Represents an 8-bit grayscale image.
 */
struct GrayImage {
    int Width = 0;
    int Height = 0;
    std::vector<std::uint8_t> Pixels;
};

}  // namespace no::image
