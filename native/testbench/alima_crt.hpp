#pragma once
#include <tls/renderer.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

class CrtOverlay {
    tls::Shader shader_;
    GLuint vao_ = 0;

    static std::string read_file(const std::string& path) {
        std::ifstream in(path, std::ios::binary);
        if (!in) throw std::runtime_error("Cannot open shader file: " + path);
        std::ostringstream text;
        text << in.rdbuf();
        return text.str();
    }

public:
    explicit CrtOverlay(const std::string& shader_dir) {
        const std::string vertex = read_file(shader_dir + "/fullscreen.vert");
        const std::string fragment = read_file(shader_dir + "/alima_crt.frag");
        shader_.compile(vertex.c_str(), fragment.c_str());
        glGenVertexArrays(1, &vao_);
    }
    ~CrtOverlay() { if (vao_) glDeleteVertexArrays(1, &vao_); }
    CrtOverlay(const CrtOverlay&) = delete;
    CrtOverlay& operator=(const CrtOverlay&) = delete;

    void draw() {
        tls::GLStateGuard guard;   
        shader_.use();
        glBindVertexArray(vao_);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};