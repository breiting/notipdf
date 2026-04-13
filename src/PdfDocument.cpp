#include "pdf/PdfDocument.hpp"

#include <mupdf/fitz.h>

namespace no::pdf {

PdfDocument::PdfDocument() = default;

PdfDocument::~PdfDocument() {
    Close();
}

PdfDocument::PdfDocument(PdfDocument&& other) noexcept {
    context_ = other.context_;
    document_ = other.document_;
    page_count_ = other.page_count_;
    path_ = std::move(other.path_);

    other.context_ = nullptr;
    other.document_ = nullptr;
    other.page_count_ = 0;
}

PdfDocument& PdfDocument::operator=(PdfDocument&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    Close();

    context_ = other.context_;
    document_ = other.document_;
    page_count_ = other.page_count_;
    path_ = std::move(other.path_);

    other.context_ = nullptr;
    other.document_ = nullptr;
    other.page_count_ = 0;

    return *this;
}

bool PdfDocument::Open(const std::string& path) {
    Close();

    context_ = fz_new_context(nullptr, nullptr, FZ_STORE_DEFAULT);
    if (!context_) {
        return false;
    }

    fz_try(context_) {
        fz_register_document_handlers(context_);
        document_ = fz_open_document(context_, path.c_str());
        page_count_ = fz_count_pages(context_, document_);
        path_ = path;
    }
    fz_catch(context_) {
        Close();
        return false;
    }

    return true;
}

void PdfDocument::Close() {
    if (context_ && document_) {
        fz_drop_document(context_, document_);
        document_ = nullptr;
    }

    if (context_) {
        fz_drop_context(context_);
        context_ = nullptr;
    }

    page_count_ = 0;
    path_.clear();
}

bool PdfDocument::IsOpen() const {
    return context_ != nullptr && document_ != nullptr;
}

int PdfDocument::GetPageCount() const {
    return page_count_;
}

std::string PdfDocument::GetPath() const {
    return path_;
}

fz_context* PdfDocument::GetContext() const {
    return context_;
}

fz_document* PdfDocument::GetHandle() const {
    return document_;
}

}  // namespace no::pdf
