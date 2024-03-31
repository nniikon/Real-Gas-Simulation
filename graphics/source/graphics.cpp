#include "graphics.h"
#include "fwd.hpp"
#include "gas_structs.h"

// static ---------------------------------------------------------------------

// #define STRINGIFY(str) #str

// shaders
static const char* vertex_shader = "#version 330 core\n"
                                    "layout (location = 0) in vec3 aPos;\n"
                                    "out vec4 Pos;\n"
                                    "void main() {\n"
                                    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                    "    Pos = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                    "}\n\0";

static const char* fragment_shader = "#version 330 core\n"
                                    "in vec4 Pos;\n"
                                    "out vec4 FragColor;\n"
                                    "void main() {\n"
                                        "FragColor = vec4(Pos.x, Pos.y, Pos.z, 1.0f);\n"
                                    "}\n\0";

// callbacks
static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int hight);
static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// debug 
static void GLLogError(size_t line);
#ifdef GRAPHICS_DEBUG_CALLS
    #define $ GLLogError(__LINE__); fprintf(stderr, "line reched: %d\n", __LINE__);
#else
    #define $ ;
#endif // GRAPHICS_DEBUG_CALLS
static void GLLogShaderError(gl_id shader_id);
static void GLLogLinkError(gl_id shader_program);

// global ---------------------------------------------------------------------
// setup
GLFWwindow* graph_SetUpRender() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(graph_kWindowHight,
                                          graph_kWindowHight,
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

    gl_id fragmet_shader_id = glCreateShader(GL_FRAGMENT_SHADER); $
    // 1 is the number of strings of source code, NULL means that strigns are null terminated
    glShaderSource(fragmet_shader_id, 1, &fragment_shader, NULL); $
    glCompileShader(fragmet_shader_id); $
    GLLogShaderError(fragmet_shader_id); $

    gl_id shader_prog_id = glCreateProgram(); $ 
    glAttachShader(shader_prog_id, vertex_shader_id); $
    glAttachShader(shader_prog_id, fragmet_shader_id); $
    glLinkProgram(shader_prog_id); $
    GLLogLinkError(shader_prog_id); $   

    glDeleteShader(vertex_shader_id); $
    glDeleteShader(fragmet_shader_id); $

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
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO); $ // vbo now is ossosiated with array buffer
    glBufferData(GL_ARRAY_BUFFER, atoms->n_coords * sizeof(float) * 3, atoms->coords, GL_STATIC_DRAW); $ // gl copy to buffer
                                                                        // mb GL_DYNAMIC_DRAW

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0); $

    glEnableVertexAttribArray(0); $
    glBindBuffer(GL_ARRAY_BUFFER, 0); $

    glBindVertexArray(0); $
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); $
    glClear(GL_COLOR_BUFFER_BIT); $
    
    glUseProgram(shader_prog_id); $
    glBindVertexArray(VAO); $
    glDrawArrays(GL_TRIANGLES, 0, 3); $
}

// static ---------------------------------------------------------------------
// callbacks

static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int hight) {
    assert(window != nullptr);

    glViewport(0, 0, width, hight);
}

static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

// debug    
static void GLLogError(size_t line) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "! Opengl error: %d, on line: %lu\n", error, line);
    }
}

static void GLLogShaderError(gl_id shader_id) {
#ifdef GRAPHICS_DEBUG_SHADER
    int success = 0;
    char log[512];
    
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success); $
    if (!success) {
        glGetShaderInfoLog(shader_id, sizeof(log), NULL, log); $
        fprintf(stderr, "# gl error: shader: %s\n", log); $
    }
#endif // GRAPHICS_DEBUG
}

static void GLLogLinkError(gl_id shader_program) {
#ifdef GRAPHICS_DEBUG_SHADER
    int success = 0;
    char log[512];
    
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success); $
    if (!success) {
        glGetProgramInfoLog(shader_program, sizeof(log), NULL, log); $
        fprintf(stderr, "# gl error: shader program: %s\n", log); $
    }
#endif // GRAPHICS_DEBUG
}