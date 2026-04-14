#include "ui/ViewerPanel.hpp"

#include <glad/gl.h>
#include <imgui.h>

namespace no::ui {

ViewerPanel::~ViewerPanel() {
    Destroy();
}

void ViewerPanel::Destroy() {
    if (m_TextureId != 0) {
        glDeleteTextures(1, &m_TextureId);
        m_TextureId = 0;
    }
    m_TextureWidth = 0;
    m_TextureHeight = 0;
}

bool ViewerPanel::EnsureTexture(const no::pdf::RenderedPage& page) {
    if (page.width <= 0 || page.height <= 0 || page.pixels_rgba.empty()) {
        return false;
    }

    if (m_TextureId == 0) {
        glGenTextures(1, &m_TextureId);
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, page.width, page.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 page.pixels_rgba.data());

    m_TextureWidth = page.width;
    m_TextureHeight = page.height;
    return true;
}

void ViewerPanel::Draw(const no::pdf::RenderedPage& page) {
    if (m_TextureId == 0) {
        ImGui::TextUnformatted("No page texture.");
        return;
    }

    ImVec2 avail = ImGui::GetContentRegionAvail();
    float img_w = static_cast<float>(page.width);
    float img_h = static_cast<float>(page.height);

    float scale = 1.0f;
    if (img_w > 0.0f && img_h > 0.0f) {
        const float sx = avail.x / img_w;
        const float sy = avail.y / img_h;
        scale = (sx < sy) ? sx : sy;
        if (scale > 1.0f) {
            scale = 1.0f;
        }
    }

    ImVec2 size(img_w * scale, img_h * scale);
    ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(m_TextureId)), size);
}

}  // namespace no::ui
