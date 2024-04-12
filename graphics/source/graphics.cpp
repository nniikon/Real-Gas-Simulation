#include "graphics.h"
#include "gas_structs.h"
#include "graphics_cfg.h"
#include "graphics_defs.h"
#include "graphics_log.h"
#include "graphics_shaders.h"

// static ----------------------------------------------------------------------

// callbacks
static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// global ----------------------------------------------------------------------
// setup
GLFWwindow* graph_SetUpRender() {
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

gl_id graph_SetUpGl() {
    gl_id vertex_shader_id = glCreateShader(GL_VERTEX_SHADER); $
    glShaderSource(vertex_shader_id, 1, &vertex_shader, NULL); $
    glCompileShader(vertex_shader_id); $
    GLLogShaderError(vertex_shader_id); $

    gl_id fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER); $
    // 1 is the number of strings of source code, 
    // NULL means that strings are null terminated
    glShaderSource(fragment_shader_id, 1, &fragment_shader, NULL); $
    glCompileShader(fragment_shader_id); $
    GLLogShaderError(fragment_shader_id); $

    gl_id shader_prog_id = glCreateProgram(); $ 
    glAttachShader(shader_prog_id, vertex_shader_id); $
    glAttachShader(shader_prog_id, fragment_shader_id); $
    glLinkProgram(shader_prog_id); $
    GLLogLinkError(shader_prog_id); $   

    glDeleteShader(vertex_shader_id); $
    glDeleteShader(fragment_shader_id); $

    glPointSize(10.0f); $ //FIXME

    return shader_prog_id;
}

// render
void Render(gas_Atoms* atoms, gl_id shader_prog_id) {
    assert(atoms != nullptr);

    gl_id VAO;
    gl_id VBO;
    
    glGenVertexArrays(1, &VAO); $ // allocate 1 vertex array with id VAO
    glGenBuffers(1, &VBO); $ // gl allocates 1 buffer with id VBO 
    glBindVertexArray(VAO); $
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO); $ // vbo now is associated with array buffer
    glBufferData(GL_ARRAY_BUFFER, 
                 (GLsizeiptr)(atoms->n_coords * sizeof(float) * 3), 
                 atoms->coords, 
                 GL_DYNAMIC_DRAW); $ // gl copy to buffer

    // why 0?
    glVertexAttribPointer(0, kNDimensions, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0); $

    glEnableVertexAttribArray(0); $
    glBindBuffer(GL_ARRAY_BUFFER, 0); $

    glBindVertexArray(0); $
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); $
    glClear(GL_COLOR_BUFFER_BIT); $
    
    glUseProgram(shader_prog_id); $
    glBindVertexArray(VAO); $
    glDrawArrays(GL_POINTS, 0, (GLsizei)atoms->n_coords); $
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
