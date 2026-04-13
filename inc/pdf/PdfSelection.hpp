#pragma once

namespace no::pdf {

/**
 * \brief Represents a rectangular selection in page coordinates.
 *
 * The coordinate system uses page space with origin at the top-left corner.
 * Width and height are always non-negative.
 */
struct PdfSelection {
    int PageIndex = 0;
    float X = 0.0f;
    float Y = 0.0f;
    float Width = 0.0f;
    float Height = 0.0f;
};

}  // namespace no::pdf
