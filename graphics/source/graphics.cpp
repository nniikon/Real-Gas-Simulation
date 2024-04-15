#include "graphics.h"
#include "fwd.hpp"
#include "gas_structs.h"
#include "glad/glad.h"
#include "graphics_cfg.h"
#include "graphics_defs.h"
#include "graphics_log.h"
#include "graphics_shaders.h"
#include <cstddef>

// static ----------------------------------------------------------------------

static const glm::vec3 kBox[] = {
    // Front face
    {-0.9f, -0.9f,  0.9f},
    { 0.9f, -0.9f,  0.9f},

    {-0.9f, -0.9f,  0.9f},
    {-0.9f,  0.9f,  0.9f},

    { 0.9f,  0.9f,  0.9f},
    { 0.9f, -0.9f,  0.9f},

    { 0.9f,  0.9f,  0.9f},
    {-0.9f,  0.9f,  0.9f},
    // Right face
};
static const size_t kNOfVertInBox = 8;

// callbacks
static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// global ----------------------------------------------------------------------
// setup
GLFWwindow* graph_SetUpRender() {
    static_assert(sizeof(glm::vec3) == sizeof(float) * 3, "FUCK GLM");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(graph_kWindowHeight,
                                          graph_kWindowHeight,
                                          graph_kWindowTitle,
                                          NULL,
                                          NULL);
    if (window == nullptr) { 
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window); 

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, graph_FrameBufferSizeCallback);
    glfwSetKeyCallback(window, graph_KeyboardCallback);

    return window;
}

GLId graph_CompileShaders() {
    GLId vertex_shader_id = glCreateShader(GL_VERTEX_SHADER); $
    glShaderSource(vertex_shader_id, 1, &vertex_shader, NULL); $
    glCompileShader(vertex_shader_id); $
    GLLogShaderError(vertex_shader_id); $

    GLId fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER); $
    // 1 is the number of strings of source code, 
    // NULL means that strings are null terminated
    glShaderSource(fragment_shader_id, 1, &fragment_shader, NULL); $
    glCompileShader(fragment_shader_id); $
    GLLogShaderError(fragment_shader_id); $

    GLId shader_prog_id = glCreateProgram(); $ 
    glAttachShader(shader_prog_id, vertex_shader_id); $
    glAttachShader(shader_prog_id, fragment_shader_id); $
    glLinkProgram(shader_prog_id); $
    GLLogLinkError(shader_prog_id); $   

    glDeleteShader(vertex_shader_id); $
    glDeleteShader(fragment_shader_id); $

    return shader_prog_id;
}

// render
void Render(gas_Atoms* atoms, GLId shader_prog_id) {
    assert(atoms != nullptr);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); $
    glClear(GL_COLOR_BUFFER_BIT); $

    GLId VAO_atoms;
    GLId VBO_atoms;
    
    glGenVertexArrays(1, &VAO_atoms); $ // allocate 1 vertex array with id VAO
    glGenBuffers(1, &VBO_atoms); $ // gl allocates 1 buffer with id VBO 
    
    glBindVertexArray(VAO_atoms); $
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_atoms); $ // vbo now is associated with array buffer
    glBufferData(GL_ARRAY_BUFFER, 
                 (GLsizeiptr)(atoms->n_coords * sizeof(float) * 3), 
                 atoms->coords, 
                 GL_DYNAMIC_DRAW); $ // gl copy to buffer

    // why 0?
    glVertexAttribPointer(0, kNDimensions, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0); $

    glBindBuffer(GL_ARRAY_BUFFER, 0); $
    glBindVertexArray(0); $
    
    glUseProgram(shader_prog_id); $
    glBindVertexArray(VAO_atoms); $
    
    glEnableVertexAttribArray(0); $
    glDrawArrays(GL_POINTS, 0, (GLsizei)atoms->n_coords); $
    glDisableVertexAttribArray(0); $

    glBindBuffer(GL_ARRAY_BUFFER, 0); $
    glBindVertexArray(0); $

    // -------------------------------------------------------------------------

    GLId VAO_box;
    GLId VBO_box;
    
    glGenVertexArrays(1, &VAO_box); $ // allocate 1 vertex array with id VAO
    glGenBuffers(1, &VBO_box); $ // gl allocates 1 buffer with id VBO 
    
    glBindVertexArray(VAO_box); $
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_box); $ // vbo now is associated with array buffer
    glBufferData(GL_ARRAY_BUFFER, 
                 sizeof(kBox), 
                 kBox, 
                 GL_STATIC_DRAW); $ // gl copy to buffer

    // why 0?
    glVertexAttribPointer(0, kNDimensions, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0); $

    glBindBuffer(GL_ARRAY_BUFFER, 0); $
    glBindVertexArray(0); $

    glUseProgram(shader_prog_id); $
    glBindVertexArray(VAO_box); $
    
    glEnableVertexAttribArray(0); $
    glDrawArrays(GL_LINES, 0, kNOfVertInBox); $
    glDisableVertexAttribArray(0); $

    glBindBuffer(GL_ARRAY_BUFFER, 0); $
    glBindVertexArray(0); $
}

// static ----------------------------------------------------------------------
// callbacks

static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    assert(window != nullptr);

    glViewport(0, 0, width, height);
}

static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
