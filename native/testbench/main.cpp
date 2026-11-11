#include <tls/renderer.hpp>
#include "azat_gl_state_checks.hpp"
#include "azat_capture.hpp"
#include "azat_frame_time.hpp"
#include "alima_crt.hpp" 
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <cstring>

int main(int argc, char** argv) {
    bool smoke = argc > 1 && std::strcmp(argv[1], "--smoke") == 0;
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto window = glfwCreateWindow(1100, 720, "THE LAST STUDENT | OpenGL 3.3 laboratory", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 2; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(smoke ? 0 : 1);

    int result = 0;
    try {
        std::string error;
        if (!tls::initialize_gl(error)) throw std::runtime_error(error);
        std::cout << "GL: " << glGetString(GL_VERSION) << " | " << glGetString(GL_RENDERER) << std::endl;

        tls::Renderer renderer;
        tls::SpriteBatch batch;

        tls::Texture2D atlas;
        atlas.load(std::string(TLS_ASSET_DIR) + "/atlas.png");

        tls::Texture2D font;
        font.load(std::string(TLS_ASSET_DIR) + "/debug_font.png");

        tls::Texture2D props;
        props.load(std::string(TLS_TESTBENCH_ASSET_DIR) + "/alima/props.png");

        tls::Texture2D lamp;
        lamp.load(std::string(TLS_TESTBENCH_ASSET_DIR) + "/alima/lamp_strip.png");

        CrtOverlay crt_overlay{ std::string(TLS_SHADER_DIR) };

        if (smoke) {
            glEnable(GL_SCISSOR_TEST); glScissor(3, 7, 11, 13);
            glEnable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
            glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
            glViewport(2, 4, 55, 60); glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, atlas.id); glClearColor(.2f, .3f, .4f, .5f);
            {
                tls::GLStateGuard guard;
                renderer.resize(320, 240);
                renderer.begin_scene({ 0, 0, 0 });
                tls::Camera2D c;
                renderer.composite(0, c, {}, { { { 0, 0 }, 100, 0, -1, 1,{ 1, 1, 1 } } }, 0, 0);
            }
            GLint active, texture, viewport[4], scissor[4], src;
            GLboolean depth_mask; GLfloat clear[4];
            glGetIntegerv(GL_ACTIVE_TEXTURE, &active);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
            glGetIntegerv(GL_VIEWPORT, viewport);
            glGetIntegerv(GL_SCISSOR_BOX, scissor);
            glGetIntegerv(GL_BLEND_SRC_RGB, &src);
            glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask);
            glGetFloatv(GL_COLOR_CLEAR_VALUE, clear);

            if (active != GL_TEXTURE3 || texture != GLint(atlas.id) || viewport[0] != 2 || viewport[2] != 55 || scissor[1] != 7 || !glIsEnabled(GL_SCISSOR_TEST) || !glIsEnabled(GL_DEPTH_TEST) || depth_mask || src != GL_SRC_ALPHA || std::abs(clear[0] - .2f) > .001f)
                throw std::runtime_error("GL state restoration failed");

            glActiveTexture(GL_TEXTURE0); glDisable(GL_SCISSOR_TEST); glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE); glBindFramebuffer(GL_FRAMEBUFFER, 0);
            std::cout << "GL STATE RESTORATION PASS" << std::endl;
            check_separate_framebuffer_bindings(renderer);
        }

        bool capture_requested = false;
        glfwSetWindowUserPointer(window, &capture_requested);
        glfwSetKeyCallback(window, [](GLFWwindow* target, int key, int, int action, int) {
            if (key == GLFW_KEY_P && action == GLFW_PRESS)
                *static_cast<bool*>(glfwGetWindowUserPointer(target)) = true;
            });

        tls::Camera2D camera;
        glm::vec2 player(320, 320);
        camera.center = player;
        float angle = 0;
        bool flashlight = true, debug = true, old_f = false, old_f3 = false;

        bool old_t = false;
        int torch_preset = 0;
        const glm::vec3 torch_presets[3] = {
            { .82f, .90f, 1.f },
            { 1.f,  .68f, .28f },
            { .35f, .85f, 1.f }
        };

        bool palette = false;
        bool old_g = false;

        bool crt = false;   
        bool old_c = false; 

        std::vector<tls::Segment> walls = {
            { { 0,0 },{ 1024,0 } },{ { 1024,0 },{ 1024,768 } },
            { { 1024,768 },{ 0,768 } },{ { 0,768 },{ 0,0 } },
            { { 480,0 },{ 480,280 } },{ { 480,430 },{ 480,768 } },
            { { 180,180 },{ 350,180 } },{ { 650,470 },{ 880,470 } }
        };

        double before = glfwGetTime();
        int frames = 0;
        double start = before;
        RollingFrameTime frame_time;

        while (!glfwWindowShouldClose(window)) {
            double now = glfwGetTime();
            double interval = now - before;
            frame_time.add(interval);
            float dt = std::min(float(interval), .05f);
            before = now;

            glfwPollEvents();
            if (glfwGetKey(window, GLFW_KEY_ESCAPE)) break;

            bool f = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
            if (f && !old_f) flashlight = !flashlight;
            old_f = f;

            bool f3 = glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS;
            if (f3 && !old_f3) debug = !debug;
            old_f3 = f3;

            bool t = glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS;
            if (t && !old_t) torch_preset = (torch_preset + 1) % 3;
            old_t = t;

            bool g = glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS;
            if (g && !old_g) palette = !palette;
            old_g = g;

            bool c_key = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
            if (c_key && !old_c) crt = !crt;
            old_c = c_key;

            glm::vec2 move(
                float(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) - float(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS),
                float(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) - float(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            );
            if (glm::length(move) > 0) {
                auto next = player + glm::normalize(move) * dt * 160.f;
                if (tls::Visibility::line_of_sight(player, next, walls)) player = next;
            }

            int w, h;
            glfwGetFramebufferSize(window, &w, &h);
            if (w < 1 || h < 1) continue;

            camera.center += (player - camera.center) * std::min(1.f, dt * 7.f);
            camera.zoom = 1.25f;

            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            auto mouse = camera.center + glm::vec2(float(mx) - w / 2.f, float(my) - h / 2.f) / camera.zoom;
            angle = std::atan2(mouse.y - player.y, mouse.x - player.x);

            renderer.resize(w, h);
            renderer.begin_scene({ .06f, .08f, .10f });

            batch.begin();
            auto tile = [&](glm::vec2 p, glm::vec2 size, int id, glm::vec4 tint = glm::vec4(1)) {
                float u = (id % 8) / 8.f, v = (id / 8) / 8.f;
                batch.quad(tls::Camera2D::model(p, size), { u, v, u + .125f, v + .125f }, tint);
                };
            const glm::mat4 world_vp = camera.projection(float(w), float(h)) * camera.view();

            auto prop = [&](glm::vec2 p, glm::vec2 size, int cell) {
                const float inset = .5f;
                float u0 = (cell * 32.f + inset) / 96.f;
                float u1 = (cell * 32.f + 32.f - inset) / 96.f;
                float v0 = inset / 32.f;
                float v1 = (32.f - inset) / 32.f;
                batch.quad(tls::Camera2D::model(p, size), { u0, v0, u1, v1 });
                };

            for (int y = 0; y < 24; ++y)
                for (int x = 0; x < 32; ++x)
                    tile({ x * 32.f + 16, y * 32.f + 16 }, { 32, 32 }, (x + y) % 3);

            for (auto wall : walls) {
                auto d = wall.b - wall.a;
                float len = glm::length(d);
                for (float s = 0; s < len; s += 32)
                    tile(wall.a + d / len * s, { 32, 32 }, 3);
            }
            batch.flush(atlas, world_vp);

            batch.begin();
            for (int i = 0; i < 3; ++i)
                prop({ 200.f + i * 100.f, 540 }, { 48, 48 }, i);
            batch.flush(props, world_vp);

            batch.begin();
            int direction = int(std::floor((angle + 3.14159f) / 1.5708f)) % 4;
            int walk = glm::length(move) > 0 ? int(now * 9) % 4 : 0;
            tile(player, { 32, 32 }, 32 + direction * 4 + walk);
            batch.flush(atlas, world_vp);

            batch.begin();
            {
                const glm::vec2 lamp_position(230.f, 130.f);
                const int lamp_frame = int(now * 6.0) % 4;
                const float inset = .5f;
                const float strip_w = float(lamp.width), strip_h = float(lamp.height);
                float u0 = (lamp_frame * 32.f + inset) / strip_w;
                float u1 = (lamp_frame * 32.f + 32.f - inset) / strip_w;
                float v0 = inset / strip_h;
                float v1 = (32.f - inset) / strip_h;
                batch.quad(tls::Camera2D::model(lamp_position, { 32, 32 }), { u0, v0, u1, v1 });
            }
            batch.flush(lamp, world_vp);

            std::vector<tls::Light> lights = {
                { { 230, 130 }, 240, 0, -1, .7f,{ .4f, .7f, .78f } },
                { { 790, 590 }, 260, 0, -1, .9f,{ .9f, .62f, .35f } }
            };

            if (flashlight)
                lights.push_back({ player, 450, angle, .75f, 2.5f, torch_presets[torch_preset] });

            renderer.palette_limit = palette;
            renderer.composite(0, camera, walls, lights, float(now), .3f);

            if (crt) crt_overlay.draw();

            if (debug) {
                char title[512];
                auto model = tls::Camera2D::model(player, { 32, 32 }, angle);
                auto v = camera.view();
                auto p = camera.projection(float(w), float(h));
                snprintf(title, sizeof(title), "OpenGL 3.3 | FPS %.0f | %.2f ms | draws %d | XY %.0f %.0f | M %.2f %.2f | V %.2f %.2f | P %.4f %.4f | WASD mouse F T G C F3 Esc", 1.f / std::max(dt, .0001f), frame_time.milliseconds(), renderer.draw_calls + 1, player.x, player.y, model[0][0], model[1][0], v[3][0], v[3][1], p[0][0], p[1][1]);
                glfwSetWindowTitle(window, title);
            }

            if (debug) {
                batch.begin();
                float row = 14;
                auto text = [&](const std::string& line) {
                    float col = 16;
                    for (unsigned char ch : line) {
                        int c = std::max(0, int(ch) - 32);
                        float u = (c % 16) / 16.f, v = (c / 16) * 10.f / 64;
                        batch.quad(tls::Camera2D::model({ col, row }, { 12, 15 }), { u, v, u + 1.f / 16, v + 10.f / 64 });
                        col += 12;
                    }
                    row += 17;
                    };
                char line[160];
                snprintf(line, sizeof(line), "GFXCORE / OPENGL 3.3   FPS %.0f   DRAWS %d   XY %.0f %.0f", 1.f / std::max(dt, .0001f), renderer.draw_calls + 2, player.x, player.y);
                text(line);
                text("WASD move / Mouse aim / F torch / T color / G palette / F3 matrices / Esc quit");
                auto matrix = [&](const char* name, const glm::mat4& m) {
                    text(name);
                    for (int r = 0; r < 4; ++r) {
                        snprintf(line, sizeof(line), " %9.3f %9.3f %9.3f %9.3f", m[0][r], m[1][r], m[2][r], m[3][r]);
                        text(line);
                    }
                    };
                matrix("PLAYER MODEL = Translate * Scale", tls::Camera2D::model(player, { 32, 32 }));
                matrix("VIEW", camera.view());
                matrix("PROJECTION", camera.projection(float(w), float(h)));
                batch.flush(font, glm::ortho(0.f, float(w), float(h), 0.f, -1.f, 1.f));
            }

            if (smoke && frames == 20) std::cout << "Screenshot: " << capture_ppm("testbench.ppm", w, h) << std::endl;
            if (capture_requested) {
                capture_requested = false;
                std::cout << "Screenshot: " << capture_ppm("testbench-manual.ppm", w, h) << std::endl;
            }
            if (smoke && ++frames >= 90) {
                glFinish();
                std::cout << "SMOKE PASS: " << frames << " frames, " << renderer.draw_calls + 2 << " draws; seconds=" << glfwGetTime() - start << std::endl;
                break;
            }
            glfwSwapBuffers(window);
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        result = 3;
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return result;
}