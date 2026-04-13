#include "pdf/PdfDocument.hpp"

#include <mupdf/fitz.h>

namespace no::pdf {

PdfDocument::PdfDocument() = default;

PdfDocument::~PdfDocument() {
    Close();
}

PdfDocument::PdfDocument(PdfDocument&& other) noexcept {
    m_Context = other.m_Context;
    m_Document = other.m_Document;
    m_PageCount = other.m_PageCount;
    m_Path = std::move(other.m_Path);

    other.m_Context = nullptr;
    other.m_Document = nullptr;
    other.m_PageCount = 0;
}

PdfDocument& PdfDocument::operator=(PdfDocument&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    Close();

    m_Context = other.m_Context;
    m_Document = other.m_Document;
    m_PageCount = other.m_PageCount;
    m_Path = std::move(other.m_Path);

    other.m_Context = nullptr;
    other.m_Document = nullptr;
    other.m_PageCount = 0;

    return *this;
}

bool PdfDocument::Open(const std::string& path) {
    Close();

    m_Context = fz_new_context(nullptr, nullptr, FZ_STORE_DEFAULT);
    if (!m_Context) {
        return false;
    }

    fz_try(m_Context) {
        fz_register_document_handlers(m_Context);
        m_Document = fz_open_document(m_Context, path.c_str());
        m_PageCount = fz_count_pages(m_Context, m_Document);
        m_Path = path;
    }
    fz_catch(m_Context) {
        Close();
        return false;
    }

    return true;
}

void PdfDocument::Close() {
    if (m_Context && m_Document) {
        fz_drop_document(m_Context, m_Document);
        m_Document = nullptr;
    }

    if (m_Context) {
        fz_drop_context(m_Context);
        m_Context = nullptr;
    }

    m_PageCount = 0;
    m_Path.clear();
}

bool PdfDocument::IsOpen() const {
    return m_Context != nullptr && m_Document != nullptr;
}

int PdfDocument::GetPageCount() const {
    return m_PageCount;
}

std::string PdfDocument::GetPath() const {
    return m_Path;
}

fz_context* PdfDocument::GetContext() const {
    return m_Context;
}

fz_document* PdfDocument::GetHandle() const {
    return m_Document;
}

bool PdfDocument::GetPageBounds(int page_index, float& out_x0, float& out_y0, float& out_x1, float& out_y1) const {
    if (!IsOpen()) {
        return false;
    }

    fz_page* page = nullptr;

    fz_try(m_Context) {
        page = fz_load_page(m_Context, m_Document, page_index);
        const fz_rect bounds = fz_bound_page(m_Context, page);

        out_x0 = bounds.x0;
        out_y0 = bounds.y0;
        out_x1 = bounds.x1;
        out_y1 = bounds.y1;
    }
    fz_always(m_Context) {
        if (page != nullptr) {
            fz_drop_page(m_Context, page);
        }
    }
    fz_catch(m_Context) {
        return false;
    }

    return true;
}

}  // namespace no::pdf
