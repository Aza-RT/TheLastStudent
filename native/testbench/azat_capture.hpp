#pragma once

#include <GL/glew.h>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// Capture the current read framebuffer before the window swaps its back buffer.
inline std::filesystem::path capture_ppm(const std::filesystem::path& filename, int width, int height) {
    std::vector<unsigned char> pixels(static_cast<std::size_t>(width) * height * 3);
    GLint previous_pack = 0;
    glGetIntegerv(GL_PACK_ALIGNMENT, &previous_pack);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT, previous_pack);
    if (glGetError() != GL_NO_ERROR) throw std::runtime_error("Cannot read pixels for " + filename.string());

    std::ofstream image(filename, std::ios::binary);
    if (!image) throw std::runtime_error("Cannot open screenshot " + filename.string());
    image << "P6\n" << width << " " << height << "\n255\n";
    for (int row = height - 1; row >= 0; --row)
        image.write(reinterpret_cast<const char*>(pixels.data()) + static_cast<std::size_t>(row) * width * 3,
                    static_cast<std::streamsize>(width) * 3);
    image.close();
    if (!image) throw std::runtime_error("Cannot write screenshot " + filename.string());
    return std::filesystem::absolute(filename);
}
