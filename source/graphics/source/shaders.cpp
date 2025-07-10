#include "graphics/shaders.hpp"

#include <stdexcept>
#include <string>
#include <fstream>
#include <cassert>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace gas {
namespace grx {

std::string LoadShader(const std::filesystem::path& shader_path) {
        std::ifstream shader_file{shader_path};
        if (!shader_file) { 
            throw std::runtime_error{"unable to open file: " + shader_path.string()};
        };

        std::ostringstream file_stream;
        file_stream << shader_file.rdbuf();

        shader_file.close();
        return file_stream.str();
}

} // namespace grx
} // namespace gas
