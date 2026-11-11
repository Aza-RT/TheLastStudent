#include <tls/renderer.hpp>
#include "shaders.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <cstddef>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
namespace tls {
    bool initialize_gl(std::string& error) {
        glewExperimental = GL_TRUE; GLenum result = glewInit();
        while (glGetError() != GL_NO_ERROR) {}
        if (result != GLEW_OK || !GLEW_VERSION_3_3) { error = "OpenGL 3.3/GLEW unavailable"; return false; }
        return true;
    }
    Shader::~Shader() { if (id_) glDeleteProgram(id_); }
    void Shader::compile(const char* vs, const char* fs) {
        std::array<GLuint, 2> stages{};
        try {
            for (int i = 0;i < 2;++i) {
                stages[i] = glCreateShader(i == 0 ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
                const char* source = i == 0 ? vs : fs; glShaderSource(stages[i], 1, &source, nullptr); glCompileShader(stages[i]);
                GLint ok; glGetShaderiv(stages[i], GL_COMPILE_STATUS, &ok);
                if (!ok) {
                    GLint length = 0;glGetShaderiv(stages[i], GL_INFO_LOG_LENGTH, &length);
                    std::vector<char> log(std::max(1, length));GLsizei written = 0;
                    glGetShaderInfoLog(stages[i], static_cast<GLsizei>(log.size()), &written, log.data());
                    throw std::runtime_error(std::string(i == 0 ? "Vertex shader compile failed: " : "Fragment shader compile failed: ") +
                        (written > 0 ? std::string(log.data(), written) : "driver returned no log"));
                }
            }
            GLuint next = glCreateProgram(); for (auto s : stages) glAttachShader(next, s); glLinkProgram(next);
            GLint ok; glGetProgramiv(next, GL_LINK_STATUS, &ok);
            if (!ok) {
                GLint length = 0;glGetProgramiv(next, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> log(std::max(1, length));GLsizei written = 0;
                glGetProgramInfoLog(next, static_cast<GLsizei>(log.size()), &written, log.data());
                glDeleteProgram(next);
                throw std::runtime_error(std::string("Program link failed: ") +
                    (written > 0 ? std::string(log.data(), written) : "driver returned no log"));
            }
            if (id_) glDeleteProgram(id_); id_ = next; uniforms_.clear();
        }
        catch (...) { for (auto s : stages) if (s) glDeleteShader(s); throw; }
        for (auto s : stages) glDeleteShader(s);
    }
    GLint Shader::uniform(const char* name) { auto it = uniforms_.find(name); if (it != uniforms_.end()) return it->second; return uniforms_.emplace(name, glGetUniformLocation(id_, name)).first->second; }
    Texture2D::Texture2D() { glGenTextures(1, &id); }
    Texture2D::~Texture2D() { glDeleteTextures(1, &id); }
    void Texture2D::allocate(int w, int h, const void* data) {
        width = w;height = h;glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    void Texture2D::load(const std::string& path) {
        int w, h, c; auto data = stbi_load(path.c_str(), &w, &h, &c, 4);
        if (!data) throw std::runtime_error("Cannot load atlas: " + path);
        allocate(w, h, data);stbi_image_free(data);
    }
    Framebuffer::Framebuffer() { glGenFramebuffers(1, &id); }
    Framebuffer::~Framebuffer() { glDeleteFramebuffers(1, &id); }
    void Framebuffer::resize(int w, int h) {
        if (color.width == w && color.height == h) return;
        color.allocate(w, h);glBindFramebuffer(GL_FRAMEBUFFER, id);glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.id, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) throw std::runtime_error("Incomplete color framebuffer");
    }
    Mesh::Mesh() { glGenVertexArrays(1, &vao);glGenBuffers(1, &vbo);glGenBuffers(1, &ebo); }
    Mesh::~Mesh() { glDeleteBuffers(1, &ebo);glDeleteBuffers(1, &vbo);glDeleteVertexArrays(1, &vao); }
    SpriteBatch::SpriteBatch() { shader_.compile(shaders::sprite_vert, shaders::sprite_frag);vertices_.reserve(32768);indices_.reserve(49152); }
    void SpriteBatch::begin() { vertices_.clear();indices_.clear(); }
    void SpriteBatch::quad(const glm::mat4& m, glm::vec4 uv, glm::vec4 color) {
        unsigned base = static_cast<unsigned>(vertices_.size());
        const glm::vec2 points[] = { { -.5f,-.5f },{ .5f,-.5f },{ .5f,.5f },{ -.5f,.5f } };
        const glm::vec2 tex[] = { { uv.x,uv.y },{ uv.z,uv.y },{ uv.z,uv.w },{ uv.x,uv.w } };
        for (int i = 0;i < 4;++i) vertices_.push_back({ glm::vec2(m * glm::vec4(points[i],0,1)),tex[i],color });
        for (unsigned i : {0u, 1u, 2u, 0u, 2u, 3u}) indices_.push_back(base + i);
    }
    void SpriteBatch::flush(const Texture2D& tex, const glm::mat4& vp) {
        shader_.use();glUniformMatrix4fv(shader_.uniform("view_projection"), 1, GL_FALSE, glm::value_ptr(vp));glUniform1i(shader_.uniform("atlas"), 0);
        glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D, tex.id);glBindVertexArray(mesh_.vao);
        glBindBuffer(GL_ARRAY_BUFFER, mesh_.vbo);glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STREAM_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_.ebo);glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned), indices_.data(), GL_STREAM_DRAW);
        glEnableVertexAttribArray(0);glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(1);glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
        glEnableVertexAttribArray(2);glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
        glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices_.size()), GL_UNSIGNED_INT, nullptr);
    }
    GLStateGuard::GLStateGuard() {
        glGetIntegerv(GL_CURRENT_PROGRAM, &program_);glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao_);glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &array_);glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &element_);
        glGetIntegerv(GL_ACTIVE_TEXTURE, &active_);glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture_);glActiveTexture(GL_TEXTURE0);glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture0_);glActiveTexture(active_);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_);glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_);glGetIntegerv(GL_VIEWPORT, viewport_);glGetIntegerv(GL_SCISSOR_BOX, scissor_);
        glGetIntegerv(GL_BLEND_SRC_RGB, &src_rgb_);glGetIntegerv(GL_BLEND_DST_RGB, &dst_rgb_);glGetIntegerv(GL_BLEND_SRC_ALPHA, &src_alpha_);glGetIntegerv(GL_BLEND_DST_ALPHA, &dst_alpha_);
        glGetIntegerv(GL_BLEND_EQUATION_RGB, &eq_rgb_);glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &eq_alpha_);
        blend_ = glIsEnabled(GL_BLEND);depth_ = glIsEnabled(GL_DEPTH_TEST);scissor_on_ = glIsEnabled(GL_SCISSOR_TEST);cull_ = glIsEnabled(GL_CULL_FACE);srgb_ = glIsEnabled(GL_FRAMEBUFFER_SRGB);
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask_);glGetBooleanv(GL_COLOR_WRITEMASK, color_mask_);
        glGetIntegerv(GL_PACK_ALIGNMENT, &pack_);glGetIntegerv(GL_UNPACK_ALIGNMENT, &unpack_);
        glGetFloatv(GL_COLOR_CLEAR_VALUE, clear_color_);
    }
    GLStateGuard::~GLStateGuard() {
        glUseProgram(program_);glBindVertexArray(vao_);glBindBuffer(GL_ARRAY_BUFFER, array_);if (vao_) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_);
        glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D, texture0_);glActiveTexture(active_);glBindTexture(GL_TEXTURE_2D, texture_);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_);glBindFramebuffer(GL_READ_FRAMEBUFFER, read_);glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);glScissor(scissor_[0], scissor_[1], scissor_[2], scissor_[3]);
        glBlendFuncSeparate(src_rgb_, dst_rgb_, src_alpha_, dst_alpha_);glBlendEquationSeparate(eq_rgb_, eq_alpha_);
        auto set = [](GLenum what, GLboolean enabled) {if (enabled) glEnable(what);else glDisable(what);};
        set(GL_BLEND, blend_);set(GL_DEPTH_TEST, depth_);set(GL_SCISSOR_TEST, scissor_on_);set(GL_CULL_FACE, cull_);set(GL_FRAMEBUFFER_SRGB, srgb_);
        glDepthMask(depth_mask_);glColorMask(color_mask_[0], color_mask_[1], color_mask_[2], color_mask_[3]);glPixelStorei(GL_PACK_ALIGNMENT, pack_);glPixelStorei(GL_UNPACK_ALIGNMENT, unpack_);
        glClearColor(clear_color_[0], clear_color_[1], clear_color_[2], clear_color_[3]);
    }
    Renderer::Renderer() { light_.compile(shaders::light_vert, shaders::light_frag);post_.compile(shaders::fullscreen_vert, shaders::post_frag);boundary_.reserve(2048);fan_.reserve(2050); }
    void Renderer::resize(int w, int h) { width = w;height = h;scene.resize(w, h);illumination.resize(w, h); }
    void Renderer::begin_scene(glm::vec3 c) { glBindFramebuffer(GL_FRAMEBUFFER, scene.id);glViewport(0, 0, width, height);glDisable(GL_DEPTH_TEST);glDisable(GL_SCISSOR_TEST);glDisable(GL_CULL_FACE);glColorMask(1, 1, 1, 1);glClearColor(c.r, c.g, c.b, 1);glClear(GL_COLOR_BUFFER_BIT);draw_calls = 0; }
    void Renderer::capture(GLuint source, int x, int y) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, source);glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D, scene.color.id);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, x, y, width, height);
    }
    void Renderer::composite(GLuint dest, const Camera2D& camera, const std::vector<Segment>& walls, const std::vector<Light>& lights, float time, float threat, int x, int y, const std::vector<glm::vec4>& ui_regions) {
        draw_calls = 0;
        glDisable(GL_SCISSOR_TEST);glDisable(GL_DEPTH_TEST);glDisable(GL_CULL_FACE);glDisable(GL_FRAMEBUFFER_SRGB);glColorMask(1, 1, 1, 1);
        glBindFramebuffer(GL_FRAMEBUFFER, illumination.id);glViewport(0, 0, width, height);glClearColor(.17f, .20f, .24f, 1);glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_BLEND);glBlendEquation(GL_FUNC_ADD);glBlendFunc(GL_ONE, GL_ONE);
        const auto vp = camera.projection(float(width), float(height)) * camera.view();
        light_.use();glUniformMatrix4fv(light_.uniform("view_projection"), 1, GL_FALSE, glm::value_ptr(vp));
        glBindVertexArray(polygon_.vao);glBindBuffer(GL_ARRAY_BUFFER, polygon_.vbo);glEnableVertexAttribArray(0);glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);
        for (const auto& l : lights) {
            Visibility::polygon(l.position, walls, l.radius, boundary_);fan_.clear();fan_.push_back(l.position);for (auto p : boundary_) fan_.push_back(p);if (!boundary_.empty()) fan_.push_back(boundary_.front());
            glBufferData(GL_ARRAY_BUFFER, fan_.size() * sizeof(glm::vec2), fan_.data(), GL_STREAM_DRAW);
            glUniform2f(light_.uniform("origin"), l.position.x, l.position.y);glUniform1f(light_.uniform("radius"), l.radius);glUniform1f(light_.uniform("angle"), l.angle);glUniform1f(light_.uniform("cone"), l.cone);glUniform1f(light_.uniform("strength"), l.strength);glUniform3fv(light_.uniform("light_color"), 1, glm::value_ptr(l.color));glUniform1f(light_.uniform("time"), time);
            glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(fan_.size()));++draw_calls;
        }
        glDisable(GL_BLEND);glBindFramebuffer(GL_FRAMEBUFFER, dest);glViewport(x, y, width, height);post_.use();
        glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D, scene.color.id);glUniform1i(post_.uniform("scene_tex"), 0);
        glActiveTexture(GL_TEXTURE1);GLint old;glGetIntegerv(GL_TEXTURE_BINDING_2D, &old);glBindTexture(GL_TEXTURE_2D, illumination.color.id);glUniform1i(post_.uniform("light_tex"), 1);
        glUniform1f(post_.uniform("time"), time);glUniform1f(post_.uniform("threat"), threat);glUniform2f(post_.uniform("resolution"), float(width), float(height));
        glUniform1i(post_.uniform("palette_limit"), palette_limit ? 1 : 0); 
        glUniform1i(post_.uniform("ui_rect_count"), static_cast<GLint>(std::min(size_t(5), ui_regions.size())));
        if (!ui_regions.empty())glUniform4fv(post_.uniform("ui_rects[0]"), static_cast<GLsizei>(std::min(size_t(5), ui_regions.size())), glm::value_ptr(ui_regions.front()));
        glBindVertexArray(fullscreen_.vao);glDrawArrays(GL_TRIANGLES, 0, 3);++draw_calls;
        glBindTexture(GL_TEXTURE_2D, old);glActiveTexture(GL_TEXTURE0);
    }
}