#include "app/Application.hpp"

#include "pdf/PdfExporter.hpp"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <imgui.h>

#include <cctype>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <vector>

#include "core/Logger.hpp"

namespace no::app {

bool Application::Initialize(const std::string& pdf_path, int viewport_width, int viewport_height) {
    LOG(Info) << "Config path: " << m_ConfigService.GetConfigPath();
    m_ConfigService.Load(m_Config);

    if (m_ConfigService.Load(m_Config)) {
        LOG(Info) << "Loaded config from: " << m_ConfigService.GetConfigPath();
    } else {
        LOG(Info) << "Using default config. Expected path: " << m_ConfigService.GetConfigPath();
    }
    if (!m_Document.Open(pdf_path)) {
        return false;
    }

    if (m_Document.GetPageCount() <= 0) {
        return false;
    }

    if (!m_ViewerRenderer.Initialize()) {
        return false;
    }

    if (!m_LineRenderer.Initialize()) {
        return false;
    }

    if (!m_PageQuad.Initialize()) {
        return false;
    }

    SetViewportSize(viewport_width, viewport_height);

    if (!LoadPage(0)) {
        return false;
    }

    m_Initialized = true;
    return true;
}

void Application::Shutdown() {
    m_Document.Close();
    m_Initialized = false;
}

void Application::RotateCCW() {
    m_RotationDegrees += 90;
    if (m_RotationDegrees >= 360) {
        m_RotationDegrees = 0;
    }

    UpdatePageTransform();
    FitCurrentPageToView();
}

void Application::UpdatePageTransform() {
    float base_width = 1.0f;
    float base_height = 1.0f;

    if (m_RenderedPage.height > 0) {
        base_width = static_cast<float>(m_RenderedPage.width) / static_cast<float>(m_RenderedPage.height);
        base_height = 1.0f;
    }

    // Keep the quad in the native page aspect ratio.
    // Rotation is applied in the renderer, not by swapping geometry dimensions here.
    m_PageQuad.SetSize(base_width, base_height);

    m_ViewerMapping.SetQuadSize(m_PageQuad.GetWidth(), m_PageQuad.GetHeight());
    m_ViewerMapping.SetRotationDegrees(m_RotationDegrees);
}

std::string Application::GetWindowTitle() const {
    std::ostringstream ss;
    ss << "notipdf"
       << " | mode: " << (m_ViewerMode == ViewerMode::Pan ? "PAN" : "SELECT") << " | page: " << (m_CurrentPageIndex + 1)
       << "/" << m_Document.GetPageCount() << " | rot: " << m_RotationDegrees;
    return ss.str();
}

void Application::ToggleViewerMode() {
    if (m_ViewerMode == ViewerMode::Pan) {
        m_ViewerMode = ViewerMode::Select;
        m_Camera.EndPan();
    } else {
        m_ViewerMode = ViewerMode::Pan;
        m_IsSelecting = false;
    }
}

void Application::OnKey(int key, int action, int /*mods*/) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
        case GLFW_KEY_ESCAPE:
            m_ShouldClose = true;
            break;

        case GLFW_KEY_SPACE:
            ToggleViewerMode();
            break;

        case GLFW_KEY_R:
            RotateCCW();
            break;

        case GLFW_KEY_N:
        case GLFW_KEY_J:
            NextPage();
            break;

        case GLFW_KEY_P:
        case GLFW_KEY_K:
            PreviousPage();
            break;

        case GLFW_KEY_DELETE:
        case GLFW_KEY_BACKSPACE:
            ClearSelections();
            break;

        case GLFW_KEY_E:
            OpenExportDialog();
            break;

        case GLFW_KEY_F:
            FitCurrentPageToView();
            break;

        case GLFW_KEY_L:
            m_IsAspectLocked = !m_IsAspectLocked;
            break;

        default:
            break;
    }
}

void Application::DrawGui() {
    DrawExportDialog();
}

void Application::OpenExportDialog() {
    if (!m_Selection.has_value()) {
        return;
    }

    m_ExportDialogState.IsOpen = true;
    m_SetFocusOnExportDialog = true;

    if (m_ExportDialogState.Title.empty()) {
        m_ExportDialogState.Title = "untitled";
    }

    m_ExportDialogState.OptimizeForEInk = m_Config.DefaultOptimizeForEInk;
    m_ExportDialogState.ThresholdToBlackAndWhite = m_Config.DefaultThresholdBlackWhite;
    m_ExportDialogState.VoiceIndex = m_Config.DefaultVoiceIndex;
}

image::ImageOptimizationSettings Application::BuildImageOptimizationSettings() const {
    image::ImageOptimizationSettings settings;
    settings.EnableWhiteClip = true;
    settings.EnableContrastStretch = true;
    settings.EnableSharpen = true;
    settings.EnableThreshold = m_ExportDialogState.ThresholdToBlackAndWhite;

    settings.WhiteClipThreshold = 235;
    settings.SharpenAmount = 0.30f;
    settings.ThresholdValue = 180;

    return settings;
}

void Application::DrawExportDialog() {
    if (!m_ExportDialogState.IsOpen) {
        return;
    }

    ImGui::OpenPopup("Export Selection");

    if (ImGui::BeginPopupModal("Export Selection", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        char title_buffer[256];
        char author_buffer[256];

        std::snprintf(title_buffer, sizeof(title_buffer), "%s", m_ExportDialogState.Title.c_str());
        std::snprintf(author_buffer, sizeof(author_buffer), "%s", m_ExportDialogState.Author.c_str());

        if (m_SetFocusOnExportDialog) {
            ImGui::SetKeyboardFocusHere();
            m_SetFocusOnExportDialog = false;
        }

        if (ImGui::InputText("Title", title_buffer, sizeof(title_buffer))) {
            m_ExportDialogState.Title = title_buffer;
        }

        if (ImGui::InputText("Author", author_buffer, sizeof(author_buffer))) {
            m_ExportDialogState.Author = author_buffer;
        }

        ImGui::TextUnformatted("Stimme");

        int voice_index = m_ExportDialogState.VoiceIndex;
        if (ImGui::RadioButton("Trompete 1", voice_index == 0)) {
            voice_index = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Trompete 2", voice_index == 1)) {
            voice_index = 1;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Fluegelhorn 1", voice_index == 2)) {
            voice_index = 2;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Fluegelhorn 2", voice_index == 3)) {
            voice_index = 3;
        }
        ImGui::Separator();

        ImGui::Checkbox("Optimize for E-Ink", &m_ExportDialogState.OptimizeForEInk);
        ImGui::Checkbox("Threshold to black/white", &m_ExportDialogState.ThresholdToBlackAndWhite);

        m_ExportDialogState.VoiceIndex = voice_index;

        ImGui::Separator();

        if (ImGui::Button("Cancel", ImVec2(120.0f, 0.0f))) {
            m_ExportDialogState.IsOpen = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Export", ImVec2(120.0f, 0.0f))) {
            if (ConfirmExport()) {
                m_ExportDialogState.IsOpen = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::EndPopup();
    }
}

bool Application::ConfirmExport() {
    if (!m_Selection.has_value()) {
        return false;
    }

    const pdf::PdfSelection& selection = *m_Selection;

    const std::string title_slug = SanitizeFileName(m_ExportDialogState.Title);
    const std::string voice_slug = SanitizeFileName(GetVoiceName(m_ExportDialogState.VoiceIndex));

    const std::string pdf_file_name = title_slug + "_" + voice_slug + ".pdf";
    const std::string json_file_name = title_slug + "_" + voice_slug + ".json";

    std::error_code ec;
    std::filesystem::create_directories(m_Config.OutputDirectory, ec);

    const std::filesystem::path output_pdf = m_Config.OutputDirectory / pdf_file_name;

    const std::filesystem::path output_json = m_Config.OutputDirectory / json_file_name;

    no::pdf::PdfExporter exporter(m_Config.Backend);

    const image::ImageOptimizationSettings optimization_settings = BuildImageOptimizationSettings();

    bool export_success =
        exporter.Export(m_Document, selection, output_pdf, no::pdf::ExportPreset::InkPad4Landscape, m_RotationDegrees,
                        m_ExportDialogState.OptimizeForEInk, optimization_settings);
    if (!export_success) {
        std::cerr << "Export failed.\n";
        return false;
    }

    pdf::MetaData meta_data;
    meta_data.Title = m_ExportDialogState.Title;
    meta_data.Author = m_ExportDialogState.Author;
    meta_data.Voice = GetVoiceName(m_ExportDialogState.VoiceIndex);
    meta_data.PdfFileName = pdf_file_name;

    if (!m_MetaWriter.Write(meta_data, output_json)) {
        std::cerr << "Writing meta.json failed.\n";
        return false;
    }

    std::cout << "Exported PDF: " << output_pdf << '\n';
    std::cout << "Exported metadata: " << output_json << '\n';

    m_Config.DefaultOptimizeForEInk = m_ExportDialogState.OptimizeForEInk;
    m_Config.DefaultThresholdBlackWhite = m_ExportDialogState.ThresholdToBlackAndWhite;
    m_Config.DefaultVoiceIndex = m_ExportDialogState.VoiceIndex;
    m_ConfigService.Save(m_Config);

    ClearSelections();

    return true;
}

std::string Application::SanitizeFileName(const std::string& value) {
    std::string result;
    result.reserve(value.size());

    for (char c : value) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            result.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        } else if (std::isspace(static_cast<unsigned char>(c)) || c == '-' || c == '_') {
            result.push_back('_');
        }
    }

    if (result.empty()) {
        result = "untitled";
    }

    std::string collapsed;
    collapsed.reserve(result.size());

    bool previous_was_underscore = false;
    for (char c : result) {
        if (c == '_') {
            if (!previous_was_underscore) {
                collapsed.push_back(c);
            }
            previous_was_underscore = true;
        } else {
            collapsed.push_back(c);
            previous_was_underscore = false;
        }
    }

    while (!collapsed.empty() && collapsed.front() == '_') {
        collapsed.erase(collapsed.begin());
    }

    while (!collapsed.empty() && collapsed.back() == '_') {
        collapsed.pop_back();
    }

    if (collapsed.empty()) {
        collapsed = "untitled";
    }

    return collapsed;
}

const char* Application::GetVoiceName(int index) {
    switch (index) {
        case 0:
            return "trompete_1";
        case 1:
            return "trompete_2";
        case 2:
            return "fluegelhorn_1";
        case 3:
            return "fluegelhorn_2";
        default:
            return "unknown";
    }
}

bool Application::ShouldClose() const {
    return m_ShouldClose;
}

void Application::FitCurrentPageToView() {
    float fit_width = m_PageQuad.GetWidth();
    float fit_height = m_PageQuad.GetHeight();

    if (m_RotationDegrees == 90 || m_RotationDegrees == 270) {
        std::swap(fit_width, fit_height);
    }

    m_Camera.FitToContent(fit_width, fit_height);
}

void Application::ClearSelections() {
    m_Selection.reset();
    m_IsSelecting = false;
    m_IsMovingSelection = false;
}

void Application::Update(float dt) {
    if (!m_Initialized) {
        return;
    }

    m_Camera.Update(dt);
}

void Application::Render() {
    m_ViewerRenderer.BeginFrame();

    if (!m_Initialized) {
        return;
    }

    m_ViewerRenderer.Draw(m_Camera, m_PageQuad, m_PageTexture, m_RotationDegrees);
    DrawSelectionOverlays();
}

void Application::DrawSelectionOverlays() const {
    const glm::vec4 persistent_color(0.95f, 0.25f, 0.2f, 1.0f);
    const glm::vec4 preview_color(0.2f, 0.9f, 0.3f, 1.0f);

    if (m_Selection.has_value()) {
        glm::vec2 min_corner;
        glm::vec2 max_corner;
        m_ViewerMapping.SelectionToWorldRect(*m_Selection, min_corner, max_corner);
        m_LineRenderer.DrawRectOutline(m_Camera, min_corner, max_corner, persistent_color);
    }

    if (m_IsSelecting) {
        const pdf::PdfSelection preview = m_ViewerMapping.MakeSelectionFromWorldDrag(
            m_CurrentPageIndex, m_SelectionStartWorld, m_SelectionCurrentWorld);

        if (preview.Width > 0.0f && preview.Height > 0.0f) {
            glm::vec2 min_corner;
            glm::vec2 max_corner;
            m_ViewerMapping.SelectionToWorldRect(preview, min_corner, max_corner);
            m_LineRenderer.DrawRectOutline(m_Camera, min_corner, max_corner, preview_color);
        }
    }
}

void Application::SetViewportSize(int width, int height) {
    m_ViewerRenderer.SetViewportSize(width, height);
    m_Camera.SetViewportSize(width, height);
}

void Application::NextPage() {
    if (!m_Document.IsOpen()) {
        return;
    }

    const int next_page = m_CurrentPageIndex + 1;
    if (next_page >= m_Document.GetPageCount()) {
        return;
    }

    LoadPage(next_page);
}

void Application::PreviousPage() {
    if (!m_Document.IsOpen()) {
        return;
    }

    const int previous_page = m_CurrentPageIndex - 1;
    if (previous_page < 0) {
        return;
    }

    LoadPage(previous_page);
}

glm::vec2 Application::GetEffectiveSelectionPoint(const glm::vec2& world) const {
    if (m_IsAspectLocked) {
        return ApplyAspectLock(m_SelectionStartWorld, world, GetActiveExportAspectRatio());
    }

    return world;
}

void Application::MoveSelectionTo(const glm::vec2& current_world) {
    if (!m_Selection.has_value()) {
        return;
    }

    const glm::vec2 page_start = m_ViewerMapping.WorldToPage(m_MoveStartWorld);
    const glm::vec2 page_current = m_ViewerMapping.WorldToPage(current_world);
    const glm::vec2 page_delta = page_current - page_start;

    const float page_x0 = m_ViewerMapping.GetPageX0();
    const float page_y0 = m_ViewerMapping.GetPageY0();
    const float page_x1 = m_ViewerMapping.GetPageX1();
    const float page_y1 = m_ViewerMapping.GetPageY1();

    pdf::PdfSelection moved = m_MoveStartSelection;

    moved.X += page_delta.x;
    moved.Y += page_delta.y;

    moved.X = std::clamp(moved.X, page_x0, page_x1 - moved.Width);
    moved.Y = std::clamp(moved.Y, page_y0, page_y1 - moved.Height);

    m_Selection = moved;
}

void Application::OnMouseButton(int button, int action, int /*mods*/, double mouse_x, double mouse_y) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    const glm::vec2 world = m_Camera.ScreenToWorld(mouse_x, mouse_y);

    if (m_ViewerMode == ViewerMode::Pan) {
        if (action == GLFW_PRESS) {
            m_Camera.BeginPan(mouse_x, mouse_y);
        } else if (action == GLFW_RELEASE) {
            m_Camera.EndPan();
        }
        return;
    }

    if (m_ViewerMode != ViewerMode::Select) {
        return;
    }

    if (action == GLFW_PRESS) {
        if (m_Selection.has_value()) {
            m_IsMovingSelection = true;
            m_MoveStartWorld = world;
            m_MoveStartSelection = *m_Selection;
            return;
        }

        if (m_ViewerMapping.IsInsidePage(world)) {
            m_IsSelecting = true;
            m_SelectionStartWorld = world;
            m_SelectionCurrentWorld = world;
        }

        return;
    }

    if (action == GLFW_RELEASE) {
        if (m_IsMovingSelection) {
            m_IsMovingSelection = false;
            return;
        }

        if (m_IsSelecting) {
            m_SelectionCurrentWorld = GetEffectiveSelectionPoint(world);

            const pdf::PdfSelection selection = m_ViewerMapping.MakeSelectionFromWorldDrag(
                m_CurrentPageIndex, m_SelectionStartWorld, m_SelectionCurrentWorld);

            if (selection.Width > 4.0f && selection.Height > 4.0f) {
                m_Selection = selection;
            }

            m_IsSelecting = false;
        }
    }
}

void Application::OnMouseMove(double mouse_x, double mouse_y) {
    if (m_ViewerMode == ViewerMode::Pan) {
        m_Camera.PanTo(mouse_x, mouse_y);
        return;
    }

    if (m_ViewerMode != ViewerMode::Select) {
        return;
    }

    const glm::vec2 world = m_Camera.ScreenToWorld(mouse_x, mouse_y);

    if (m_IsMovingSelection) {
        MoveSelectionTo(world);
        return;
    }

    if (m_IsSelecting) {
        m_SelectionCurrentWorld = GetEffectiveSelectionPoint(world);
    }
}

void Application::OnScroll(double /*xoffset*/, double yoffset, double mouse_x, double mouse_y) {
    m_Camera.ZoomAt(yoffset, mouse_x, mouse_y);
}

float Application::GetActiveExportAspectRatio() const {
    // TODO: fix it
    return 1872.0f / 1404.0f;
}

glm::vec2 Application::ApplyAspectLock(const glm::vec2& anchor_world, const glm::vec2& current_world,
                                       float aspect_ratio) const {
    const glm::vec2 delta = current_world - anchor_world;

    const float sign_x = (delta.x < 0.0f) ? -1.0f : 1.0f;
    const float sign_y = (delta.y < 0.0f) ? -1.0f : 1.0f;

    const float abs_dx = std::abs(delta.x);
    const float abs_dy = std::abs(delta.y);

    if (abs_dx < 1e-6f && abs_dy < 1e-6f) {
        return current_world;
    }

    float locked_w = abs_dx;
    float locked_h = abs_dy;

    if (abs_dy < 1e-6f) {
        locked_h = locked_w / aspect_ratio;
    } else if (abs_dx < 1e-6f) {
        locked_w = locked_h * aspect_ratio;
    } else {
        const float current_aspect = abs_dx / abs_dy;

        if (current_aspect > aspect_ratio) {
            locked_h = abs_dx / aspect_ratio;
            locked_w = abs_dx;
        } else {
            locked_w = abs_dy * aspect_ratio;
            locked_h = abs_dy;
        }
    }

    return {anchor_world.x + sign_x * locked_w, anchor_world.y + sign_y * locked_h};
}

bool Application::LoadPage(int page_index) {
    if (!m_PdfRenderer.RenderPage(m_Document, page_index, 2.0f, m_RenderedPage)) {
        return false;
    }

    if (!m_PageTexture.UploadRGBA(m_RenderedPage.width, m_RenderedPage.height, m_RenderedPage.pixels_rgba.data())) {
        return false;
    }

    float page_x0 = 0.0f;
    float page_y0 = 0.0f;
    float page_x1 = 0.0f;
    float page_y1 = 0.0f;
    if (!m_Document.GetPageBounds(page_index, page_x0, page_y0, page_x1, page_y1)) {
        return false;
    }

    m_ViewerMapping.SetPageBounds(page_x0, page_y0, page_x1, page_y1);

    UpdatePageTransform();
    FitCurrentPageToView();

    m_CurrentPageIndex = page_index;
    ClearSelections();

    return true;
}

}  // namespace no::app
