#include "graphics/graphics_shaders.hpp"

#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <sstream>
#include <iostream>

namespace gas {
namespace grx {

std::vector<std::string> LoadShaders(std::vector<std::string> file_names_arr) {
    std::vector<std::string> shader_arr;

    for (auto file_name: file_names_arr) {
        std::ifstream shader_file(file_name);
        if (!shader_file) { assert(0 && "unable to open file"); }

        std::ostringstream file_stream;
        file_stream << shader_file.rdbuf();
        shader_arr.push_back(file_stream.str());

        shader_file.close();
    }

    return shader_arr;
}

} // namespace grx
} // namespace gas
