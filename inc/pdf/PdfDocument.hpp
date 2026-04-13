#pragma once

#include <string>

struct fz_context;
struct fz_document;
struct fz_page;

namespace no::pdf {

class PdfDocument {
   public:
    PdfDocument();
    ~PdfDocument();

    PdfDocument(const PdfDocument&) = delete;
    PdfDocument& operator=(const PdfDocument&) = delete;

    PdfDocument(PdfDocument&& other) noexcept;
    PdfDocument& operator=(PdfDocument&& other) noexcept;

    /**
     * \brief Returns the page bounds in PDF page coordinates.
     *
     * \param page_index The zero-based page index.
     * \param out_x0 Left page bound.
     * \param out_y0 Top page bound.
     * \param out_x1 Right page bound.
     * \param out_y1 Bottom page bound.
     * \return True on success, false otherwise.
     */
    bool GetPageBounds(int page_index, float& out_x0, float& out_y0, float& out_x1, float& out_y1) const;

    bool Open(const std::string& path);
    void Close();

    bool IsOpen() const;
    int GetPageCount() const;
    std::string GetPath() const;

    fz_context* GetContext() const;
    fz_document* GetHandle() const;

   private:
    fz_context* m_Context = nullptr;
    fz_document* m_Document = nullptr;
    int m_PageCount = 0;
    std::string m_Path;
};

}  // namespace no::pdf
