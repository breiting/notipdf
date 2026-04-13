#include "render/PdfViewerRenderer.hpp"

#include <glad/gl.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "render/Camera2D.hpp"
#include "render/PdfQuad.hpp"
#include "render/PdfTexture.hpp"

namespace no::render {

namespace {
constexpr const char* kVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;

uniform mat4 u_mvp;

out vec2 v_texcoord;

void main() {
    v_texcoord = a_texcoord;
    gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);
}
)";

constexpr const char* kFragmentShader = R"(
#version 330 core
in vec2 v_texcoord;
out vec4 frag_color;

uniform sampler2D u_texture0;

void main() {
    frag_color = texture(u_texture0, v_texcoord);
}
)";
}  // namespace

PdfViewerRenderer::PdfViewerRenderer() : m_Program(0), m_UMvp(-1), m_UTexture(-1), m_ViewportSize(1600, 1000) {
}

PdfViewerRenderer::~PdfViewerRenderer() {
    if (m_Program != 0) {
        glDeleteProgram(m_Program);
    }
}

bool PdfViewerRenderer::Initialize() {
    const unsigned int vs = CompileShader(GL_VERTEX_SHADER, kVertexShader);
    if (vs == 0) {
        return false;
    }

    const unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, kFragmentShader);
    if (fs == 0) {
        glDeleteShader(vs);
        return false;
    }

    const bool linked = LinkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    if (!linked) {
        return false;
    }

    m_UMvp = glGetUniformLocation(m_Program, "u_mvp");
    m_UTexture = glGetUniformLocation(m_Program, "u_texture0");
    return m_UMvp >= 0 && m_UTexture >= 0;
}

void PdfViewerRenderer::SetViewportSize(int width, int height) {
    m_ViewportSize.x = width;
    m_ViewportSize.y = height;
}

void PdfViewerRenderer::BeginFrame() const {
    glViewport(0, 0, m_ViewportSize.x, m_ViewportSize.y);
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PdfViewerRenderer::Draw(const Camera2D& camera, const PdfQuad& quad, const PdfTexture& texture,
                             int rotation_degrees) const {
    if (m_Program == 0 || !texture.IsValid()) {
        return;
    }

    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(static_cast<float>(rotation_degrees)), glm::vec3(0.0f, 0.0f, 1.0f));

    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 proj = camera.GetProjectionMatrix();
    const glm::mat4 mvp = proj * view * model;

    glUseProgram(m_Program);
    glUniformMatrix4fv(m_UMvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(m_UTexture, 0);

    texture.Bind(0);
    quad.Render();

    glUseProgram(0);
}

unsigned int PdfViewerRenderer::CompileShader(unsigned int type, const char* source) const {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool PdfViewerRenderer::LinkProgram(unsigned int vertex_shader, unsigned int fragment_shader) {
    m_Program = glCreateProgram();
    glAttachShader(m_Program, vertex_shader);
    glAttachShader(m_Program, fragment_shader);
    glLinkProgram(m_Program);

    int success = 0;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glDeleteProgram(m_Program);
        m_Program = 0;
        return false;
    }

    return true;
}

}  // namespace no::render
