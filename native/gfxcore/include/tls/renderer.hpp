#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tls/visibility.hpp>
#include <string>
#include <unordered_map>
#include <vector>
namespace tls {
    class Shader {
        GLuint id_ = 0; std::unordered_map<std::string, GLint> uniforms_;
    public:
        Shader() = default; ~Shader(); Shader(const Shader&) = delete; Shader& operator=(const Shader&) = delete;
        void compile(const char* vertex, const char* fragment);
        void use() const { glUseProgram(id_); }
        GLint uniform(const char* name);
    };
    class Texture2D {
    public:
        GLuint id = 0; int width = 0, height = 0;
        Texture2D(); ~Texture2D(); Texture2D(const Texture2D&) = delete; Texture2D& operator=(const Texture2D&) = delete;
        void allocate(int w, int h, const void* pixels = nullptr);
        void load(const std::string& path);
    };
    class Framebuffer {
    public:
        GLuint id = 0; Texture2D color;
        Framebuffer(); ~Framebuffer(); Framebuffer(const Framebuffer&) = delete;
        void resize(int w, int h);
    };
    class Mesh {
    public:
        GLuint vao = 0, vbo = 0, ebo = 0;
        Mesh(); ~Mesh(); Mesh(const Mesh&) = delete;
    };
    struct Camera2D {
        glm::vec2 center{ 0 }; float zoom = 1;
        glm::mat4 view() const { return glm::scale(glm::mat4(1), glm::vec3(zoom, zoom, 1)) * glm::translate(glm::mat4(1), glm::vec3(-center, 0)); }
        glm::mat4 projection(float w, float h) const { return glm::ortho(-w / 2, w / 2, h / 2, -h / 2, -1.f, 1.f); }
        static glm::mat4 model(glm::vec2 p, glm::vec2 size, float angle = 0) {
            return glm::translate(glm::mat4(1), glm::vec3(p, 0)) * glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1)) * glm::scale(glm::mat4(1), glm::vec3(size, 1));
        }
    };
    struct Vertex { glm::vec2 position, uv; glm::vec4 color; };
    class SpriteBatch {
        Mesh mesh_; Shader shader_; std::vector<Vertex> vertices_; std::vector<unsigned> indices_;
    public:
        SpriteBatch();
        void begin();
        void quad(const glm::mat4& model, glm::vec4 uv, glm::vec4 color = glm::vec4(1));
        void flush(const Texture2D& atlas, const glm::mat4& vp);
    };
    class GLStateGuard {
        GLint program_, vao_, array_, element_, active_, texture_, texture0_, draw_, read_, viewport_[4], scissor_[4];
        GLint src_rgb_, dst_rgb_, src_alpha_, dst_alpha_, eq_rgb_, eq_alpha_, pack_, unpack_;
        GLfloat clear_color_[4];
        GLboolean blend_, depth_, scissor_on_, cull_, srgb_, depth_mask_, color_mask_[4];
    public:
        GLStateGuard(); ~GLStateGuard();
    };
    struct Light { glm::vec2 position; float radius = 300, angle = 0, cone = -1, strength = 1; glm::vec3 color{ 1 }; };
    class Renderer {
        Shader light_, post_; Mesh polygon_, fullscreen_;
        std::vector<glm::vec2> boundary_, fan_;
    public:
        Framebuffer scene, illumination;
        int width = 0, height = 0, draw_calls = 0;
        bool palette_limit = false;
        Renderer();
        void resize(int w, int h);
        void begin_scene(glm::vec3 clear);
        void capture(GLuint source, int x, int y);
        void composite(GLuint destination, const Camera2D& camera, const std::vector<Segment>& walls, const std::vector<Light>& lights, float time, float threat, int x = 0, int y = 0, const std::vector<glm::vec4>& ui_regions = {});
    };
    bool initialize_gl(std::string& error);
}