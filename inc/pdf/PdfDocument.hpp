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
