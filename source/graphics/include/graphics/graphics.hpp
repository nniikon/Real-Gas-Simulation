#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_

#include <memory>
#include <iostream>
#include <string>
#include <cstdint>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "common/gas_structs.hpp"

#include "graphics/graphics_cfg.hpp"
#include "graphics/graphics_defs.hpp"

namespace gas {
namespace grx {

class GraphicsContext {
  private:
    inline static int64_t n_instances = 0;
  public:
    GraphicsContext() {
        if (n_instances != 0) {
            return ;
        }
        
        n_instances++;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kOpenGLMajorVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kOpenGLMinorVersion);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    ~GraphicsContext() {
        if (n_instances != 1) {
            return ;
        }

        n_instances--;

        glfwTerminate();
    }
};

struct GraphShaders {
    GLId main_shader_program_id;
    GLId box_shader_program_id;
};

inline void WindowDeleter(GLFWwindow* window) {
    // std::cerr << "window deleter called" << std::endl;
    if (window == nullptr) {
        // std::cerr << "window nullptr passed" << std::endl;
        return ;
    }

    glfwDestroyWindow(window);
}
using WindowHandle = std::unique_ptr<GLFWwindow, decltype(&WindowDeleter)>;

// setup
[[nodiscard]]
WindowHandle SetUpRender();

// control dump
[[nodiscard]]
std::string TellAboutControls();

// returns shader program handle
[[nodiscard]]
GraphShaders CompileShaders();

void Render(gas_Atoms* atoms, const GraphShaders& shader_ids);

} // namespace grx
} // namespace gas

#endif // GRAPHICS_HPP_
