#pragma once

#include <glm/glm.hpp>

#include "pdf/PdfSelection.hpp"

namespace no::render {

/**
 * \brief Provides coordinate mapping between screen, world, quad, and page space.
 */
class ViewerMapping {
   public:
    /**
     * \brief Sets the PDF page bounds in page coordinate space.
     *
     * \param x0 Left page bound.
     * \param y0 Top page bound.
     * \param x1 Right page bound.
     * \param y1 Bottom page bound.
     */
    void SetPageBounds(float x0, float y0, float x1, float y1);

    /**
     * \brief Sets the quad size in world units.
     *
     * \param quad_width The quad width in world units.
     * \param quad_height The quad height in world units.
     */
    void SetQuadSize(float quad_width, float quad_height);

    /**
     * \brief Converts a world-space point to quad-local coordinates.
     *
     * The quad is centered at the origin.
     *
     * \param world The input world-space point.
     * \return The local quad coordinates.
     */
    glm::vec2 WorldToQuadLocal(const glm::vec2& world) const;

    /**
     * \brief Tests whether a world-space point lies inside the page quad.
     *
     * \param world The input world-space point.
     * \return True if inside the quad, false otherwise.
     */
    bool IsInsidePage(const glm::vec2& world) const;

    /**
     * \brief Converts two world-space drag points into a page selection.
     *
     * \param page_index The page index.
     * \param world_a The drag start point in world space.
     * \param world_b The drag end point in world space.
     * \return The normalized page-space selection.
     */
    pdf::PdfSelection MakeSelectionFromWorldDrag(int page_index, const glm::vec2& world_a,
                                                 const glm::vec2& world_b) const;

    /**
     * \brief Converts a page selection back to quad/world rectangle corners.
     *
     * \param selection The page selection.
     * \param out_min The minimum world corner.
     * \param out_max The maximum world corner.
     */
    void SelectionToWorldRect(const pdf::PdfSelection& selection, glm::vec2& out_min, glm::vec2& out_max) const;

   private:
    float m_PageX0 = 0.0f;
    float m_PageY0 = 0.0f;
    float m_PageX1 = 1.0f;
    float m_PageY1 = 1.0f;
    float m_QuadWidth = 1.0f;
    float m_QuadHeight = 1.0f;
};

}  // namespace no::render
