#include "ui/ViewerPanel.hpp"

#include <glad/gl.h>
#include <imgui.h>

namespace no::ui {

ViewerPanel::~ViewerPanel() {
    if (texture_id_ != 0) {
        glDeleteTextures(1, &texture_id_);
        texture_id_ = 0;
    }
}

bool ViewerPanel::EnsureTexture(const no::pdf::RenderedPage& page) {
    if (page.width <= 0 || page.height <= 0 || page.pixels_rgba.empty()) {
        return false;
    }

    if (texture_id_ == 0) {
        glGenTextures(1, &texture_id_);
        glBindTexture(GL_TEXTURE_2D, texture_id_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        glBindTexture(GL_TEXTURE_2D, texture_id_);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, page.width, page.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 page.pixels_rgba.data());

    texture_width_ = page.width;
    texture_height_ = page.height;
    return true;
}

void ViewerPanel::Draw(const no::pdf::RenderedPage& page) {
    if (texture_id_ == 0) {
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
    ImGui::Image(static_cast<ImTextureID>(static_cast<intptr_t>(texture_id_)), size);
}

}  // namespace no::ui
