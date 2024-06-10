#include "graphics.h"

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include "fwd.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/string_cast.hpp"

#include "gas_structs.h"
#include "glad/glad.h"
#include "graphics_cfg.h"
#include "graphics_defs.h"
#include "graphics_log.h"
#include "graphics_shaders.h"
#include "graphics_box.h"

// static ----------------------------------------------------------------------

static glm::ivec3 angle_vec(0, 0, 0);
static float scale_scene = 0.5f;

// callbacks
static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static GLId graph_CompileProgram(const char* vert_shader, const char* frag_shader);

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

GraphShaders graph_CompileShaders() {
    // load shaders
    std::vector<std::string> file_names_arr;
    file_names_arr.push_back(std::string("./graphics/shaders/main_frag_sh.frag"));
    file_names_arr.push_back(std::string("./graphics/shaders/main_vert_sh.vert"));

    file_names_arr.push_back(std::string("./graphics/shaders/box_frag_sh.frag"));
    file_names_arr.push_back(std::string("./graphics/shaders/box_vert_sh.vert"));

    std::vector<std::string> shader_arr = graph_LoadShaders(file_names_arr);
    
    int64_t main_frag_ind = 0;
    int64_t main_vert_ind = 1;
    int64_t box_frag_ind  = 2;
    int64_t box_vert_ind  = 3;

    const char* main_frag_shader = shader_arr[main_frag_ind].c_str();
    const char* main_vert_shader = shader_arr[main_vert_ind].c_str();
    const char* box_frag_shader  = shader_arr[box_frag_ind].c_str();
    const char* box_vert_shader  = shader_arr[box_vert_ind].c_str(); 

    GLId main_shader_program_id = graph_CompileProgram(main_vert_shader, main_frag_shader);
    GLId box_shader_program_id = graph_CompileProgram(box_vert_shader, box_frag_shader);

    return (GraphShaders){.main_shader_program_id = main_shader_program_id,
                          .box_shader_program_id  = box_shader_program_id};
}

// render
void Render(gas_Atoms* atoms, const GraphShaders& shader_ids) {
    assert(atoms != nullptr);

    glm::mat4 rotate_mat_x = glm::mat4(1.0f);
    glm::mat4 rotate_mat_y = glm::mat4(1.0f);
    glm::mat4 rotate_mat_z = glm::mat4(1.0f);

    rotate_mat_x = glm::rotate(rotate_mat_x, glm::radians((float)angle_vec.x), glm::vec3(1.0, 0.0, 0.0));
    rotate_mat_y = glm::rotate(rotate_mat_y, glm::radians((float)angle_vec.y), glm::vec3(0.0, 1.0, 0.0));
    rotate_mat_z = glm::rotate(rotate_mat_z, glm::radians((float)angle_vec.z), glm::vec3(0.0, 0.0, 1.0));

    glm::mat4 rotate_mat = rotate_mat_x * rotate_mat_y * rotate_mat_z;
    // rotate_mat = glm::rotate(rotate_mat, glm::radians((float)angle), glm::vec3(0.0, 1.0, 1.0));

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

    glUseProgram(shader_ids.main_shader_program_id); $
    
    int main_rotate_id = glGetUniformLocation(shader_ids.main_shader_program_id, "rotate_mat"); $
    glUniformMatrix4fv(main_rotate_id, 1, GL_FALSE, glm::value_ptr(rotate_mat)); $

    int main_scale_id = glad_glGetUniformLocation(shader_ids.main_shader_program_id, "scale_scene");
    glUniform1f(main_scale_id, scale_scene);

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

    glUseProgram(shader_ids.box_shader_program_id); $

    int box_rotate_id = glGetUniformLocation(shader_ids.box_shader_program_id, "rotate_mat"); $
    glUniformMatrix4fv(box_rotate_id, 1, GL_FALSE, glm::value_ptr(rotate_mat)); $
    
    int box_scale_id = glad_glGetUniformLocation(shader_ids.box_shader_program_id, "scale_scene");
    glUniform1f(box_scale_id, scale_scene);

    glBindVertexArray(VAO_box); $
        
    glEnableVertexAttribArray(0); $
    glDrawArrays(GL_LINES, 0, kNOfVertInBox); $
    glDisableVertexAttribArray(0); $

    glBindBuffer(GL_ARRAY_BUFFER, 0); $
    glBindVertexArray(0); $
}

// static ----------------------------------------------------------------------

static GLId graph_CompileProgram(const char* vert_shader, const char* frag_shader) {
    assert(vert_shader != nullptr);
    assert(frag_shader != nullptr);

    GLId vertex_shader_id = glCreateShader(GL_VERTEX_SHADER); $
    glShaderSource(vertex_shader_id, 1, &vert_shader, NULL); $
    glCompileShader(vertex_shader_id); $
    GLLogShaderError(vertex_shader_id); $

    GLId fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER); $
    // 1 is the number of strings of source code, 
    // NULL means that strings are null terminated
    glShaderSource(fragment_shader_id, 1, &frag_shader, NULL); $
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

void graph_TellAboutControls() {
    std::cout << "Quit: Q" << std::endl
              << "Rotate +X: Z" << std::endl
              << "Rotate +Y: X" << std::endl
              << "Rotate +Z: C" << std::endl
              << "Rotate -X: A" << std::endl
              << "Rotate -Y: S" << std::endl
              << "Rotate -Z: D" << std::endl
              << "Dump velocities: V" << std::endl;
}

// callbacks

static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    assert(window != nullptr);

    glViewport(0, 0, width, height);
}

static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (key) {
        case GLFW_KEY_Q:
           glfwSetWindowShouldClose(window, GLFW_TRUE); 
        case GLFW_KEY_Z:
            angle_vec.x++;
            // angle_vec.x %= 360;
            break;
        case GLFW_KEY_X:
            angle_vec.y++;
            // angle_vec.y %= 360;
            break;
        case GLFW_KEY_C:
            angle_vec.z++;
            // angle_vec.z %= 360;
            break;
        case GLFW_KEY_A:
            angle_vec.x--;
            // angle_vec.x %= 360;
            break;
        case GLFW_KEY_S:
            angle_vec.y--;
            // angle_vec.y %= 360;
            break;
        case GLFW_KEY_D:
            angle_vec.z--;
            // angle_vec.z %= 360;            
            break;
        case GLFW_KEY_V:
            // dump v
            break;
        
        case GLFW_KEY_EQUAL:
            scale_scene *= 1.05f;
            break;
        case GLFW_KEY_MINUS:
            scale_scene /= 1.05f;
            break;
        default:
        #if defined(GRAPH_DEBUG)
            std::cerr << "! unknown key: " << key << std::endl;
        #endif // DEBUG
            break;
    }

#if defined (GRAPH_DEBUG)
    std::cerr << "# angle vector: " << glm::to_string(angle_vec) << std::endl;
    std::cerr << "# scale float: " << scale_scene << std::endl;
#endif // GRAPH_DEBUG
}
