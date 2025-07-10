#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_

#include <memory>
#include <iostream>
#include <string>
#include <cstdint>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "common/gas_structs.hpp"

#include "glm/fwd.hpp"
#include "graphics/vertex.hpp"
#include "graphics/config.hpp"
#include "graphics/shaders.hpp"

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

// main program
[[nodiscard]]
ShaderProgram GetMainShaderProgram();

// box program 
[[nodiscard]]
ShaderProgram GetBoxShaderProgram();

[[nodiscard]]
size_t BoxVertSize();

class RenderEngine {
  private:
    WindowHandle window_handle_;

    ShaderProgram main_program_;
    VertexArray main_vao_;
    VertexBuffer main_vbo_;
    
    ShaderProgram box_program_;
    VertexArray box_vao_;
    VertexBuffer box_vbo_;  
  public:
    RenderEngine(size_t n_atoms, size_t n_box_vert) 
        : window_handle_{SetUpRender()},  
        main_program_{GetMainShaderProgram()},
        main_vao_{},
        main_vbo_{VertexBuffer::Type::kArrayBuffer},
        box_program_{GetBoxShaderProgram()},
        box_vao_{},
        box_vbo_{VertexBuffer::Type::kArrayBuffer}
    {
        main_vao_.Bind();
        main_vbo_.Bind();

        main_vbo_.Allocate(
            n_atoms * sizeof(glm::vec3), 
            GL_DYNAMIC_DRAW
        );

        main_vbo_.Unbind();
        main_vao_.Unbind();

        box_vao_.Bind();
        box_vbo_.Bind();

        box_vbo_.Allocate(
            n_box_vert * sizeof(glm::vec3), 
            GL_DYNAMIC_DRAW
        );

        box_vbo_.Unbind();
        box_vao_.Unbind();
    }
    
    bool IsFinished() {
        return glfwWindowShouldClose(window_handle_.get());
    }

    void Prologue() {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GlDbg();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GlDbg();
    }

    void Epilogue() {
        glfwSwapBuffers(window_handle_.get());
        glfwPollEvents();
    }

    void Render(gas_Atoms* atoms);
};

} // namespace grx
} // namespace gas

#endif // GRAPHICS_HPP_
