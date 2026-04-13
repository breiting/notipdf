#include "render/PdfViewerRenderer.hpp"

#include <glad/gl.h>

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

PdfViewerRenderer::PdfViewerRenderer() : program_(0), u_mvp_(-1), u_texture_(-1), viewport_size_(1600, 1000) {
}

PdfViewerRenderer::~PdfViewerRenderer() {
    if (program_ != 0) {
        glDeleteProgram(program_);
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

    u_mvp_ = glGetUniformLocation(program_, "u_mvp");
    u_texture_ = glGetUniformLocation(program_, "u_texture0");
    return u_mvp_ >= 0 && u_texture_ >= 0;
}

void PdfViewerRenderer::SetViewportSize(int width, int height) {
    viewport_size_.x = width;
    viewport_size_.y = height;
}

void PdfViewerRenderer::BeginFrame() const {
    glViewport(0, 0, viewport_size_.x, viewport_size_.y);
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PdfViewerRenderer::Draw(const Camera2D& camera, const PdfQuad& quad, const PdfTexture& texture) const {
    if (program_ == 0 || !texture.IsValid()) {
        return;
    }

    const glm::mat4 model(1.0f);
    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 proj = camera.GetProjectionMatrix();
    const glm::mat4 mvp = proj * view * model;

    glUseProgram(program_);
    glUniformMatrix4fv(u_mvp_, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(u_texture_, 0);

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
    program_ = glCreateProgram();
    glAttachShader(program_, vertex_shader);
    glAttachShader(program_, fragment_shader);
    glLinkProgram(program_);

    int success = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glDeleteProgram(program_);
        program_ = 0;
        return false;
    }

    return true;
}

}  // namespace no::render
