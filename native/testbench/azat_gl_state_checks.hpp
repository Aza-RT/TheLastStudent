#pragma once

#include <tls/renderer.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

inline void check_separate_framebuffer_bindings(tls::Renderer& renderer) {
    // Save the normal loop's GL state before creating either temporary target.
    tls::GLStateGuard restore_initial;
    tls::Framebuffer read_target, draw_target;
    read_target.resize(2, 2);
    draw_target.resize(2, 2);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, read_target.id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_target.id);

    GLint expected_read = 0, expected_draw = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &expected_read);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &expected_draw);
    if (expected_read == expected_draw || expected_read != static_cast<GLint>(read_target.id) ||
        expected_draw != static_cast<GLint>(draw_target.id))
        throw std::runtime_error("Could not establish distinct read and draw framebuffer bindings");

    {
        tls::GLStateGuard guarded_pass;
        renderer.begin_scene({0, 0, 0});
        tls::Camera2D camera;
        renderer.composite(0, camera, {}, {}, 0, 0);
    }

    GLint actual_read = 0, actual_draw = 0;
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &actual_read);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &actual_draw);
    if (actual_read != expected_read || actual_draw != expected_draw)
        throw std::runtime_error("Framebuffer binding restoration failed: read expected " +
            std::to_string(expected_read) + ", actual " + std::to_string(actual_read) +
            "; draw expected " + std::to_string(expected_draw) + ", actual " +
            std::to_string(actual_draw));

    std::cout << "SEPARATE FRAMEBUFFER RESTORATION PASS: read=" << actual_read
              << " draw=" << actual_draw << std::endl;
}
