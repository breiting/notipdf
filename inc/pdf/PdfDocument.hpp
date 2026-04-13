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
    fz_context* context_ = nullptr;
    fz_document* document_ = nullptr;
    int page_count_ = 0;
    std::string path_;
};

}  // namespace no::pdf
