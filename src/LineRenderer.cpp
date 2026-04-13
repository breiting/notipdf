#include "render/LineRenderer.hpp"

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

#include "render/Camera2D.hpp"

namespace no::render {

namespace {
constexpr const char* kVertexShader = R"(
#version 330 core
layout(location = 0) in vec2 a_position;

uniform mat4 u_mvp;

void main() {
    gl_Position = u_mvp * vec4(a_position, 0.0, 1.0);
}
)";

constexpr const char* kFragmentShader = R"(
#version 330 core
uniform vec4 u_color;
out vec4 frag_color;

void main() {
    frag_color = u_color;
}
)";
}  // namespace

LineRenderer::LineRenderer() = default;

LineRenderer::~LineRenderer() {
    if (m_Vbo != 0) {
        glDeleteBuffers(1, &m_Vbo);
    }
    if (m_Vao != 0) {
        glDeleteVertexArrays(1, &m_Vao);
    }
    if (m_Program != 0) {
        glDeleteProgram(m_Program);
    }
}

bool LineRenderer::Initialize() {
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

    m_UniformMvp = glGetUniformLocation(m_Program, "u_mvp");
    m_UniformColor = glGetUniformLocation(m_Program, "u_color");
    if (m_UniformMvp < 0 || m_UniformColor < 0) {
        return false;
    }

    glGenVertexArrays(1, &m_Vao);
    glGenBuffers(1, &m_Vbo);

    if (m_Vao == 0 || m_Vbo == 0) {
        return false;
    }

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(0));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return glGetError() == GL_NO_ERROR;
}

void LineRenderer::DrawRectOutline(const Camera2D& camera, const glm::vec2& min_corner, const glm::vec2& max_corner,
                                   const glm::vec4& color) const {
    if (m_Program == 0) {
        return;
    }

    const float vertices[8] = {min_corner.x, min_corner.y, max_corner.x, min_corner.y,
                               max_corner.x, max_corner.y, min_corner.x, max_corner.y};

    const glm::mat4 model(1.0f);
    const glm::mat4 view = camera.GetViewMatrix();
    const glm::mat4 proj = camera.GetProjectionMatrix();
    const glm::mat4 mvp = proj * view * model;

    glUseProgram(m_Program);
    glUniformMatrix4fv(m_UniformMvp, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform4fv(m_UniformColor, 1, glm::value_ptr(color));

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_Vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_LINE_LOOP, 0, 4);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

unsigned int LineRenderer::CompileShader(unsigned int type, const char* source) const {
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

bool LineRenderer::LinkProgram(unsigned int vertex_shader, unsigned int fragment_shader) {
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
