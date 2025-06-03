#include "graphics/graphics.hpp"

#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include <numbers>

#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/gl.h>

#include "common/gas_structs.h"

#include "graphics/graphics_defs.hpp"
#include "graphics/graphics_log.hpp"
#include "graphics/graphics_shaders.hpp"
#include "graphics/graphics_box.hpp"

#include "debug/dbg.h"

// static ----------------------------------------------------------------------

static glm::ivec3 angle_vec(0, 0, 0);
static float scale_scene = 0.5f;
extern float radius_global;

// callbacks
static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
static void graph_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static glm::mat4 graph_GetRotationMatrix();
static GLId graph_CompileProgram(const char* vert_shader, const char* frag_shader);
static void graph_CreateCircle(std::vector<glm::vec3>* box);

// global ----------------------------------------------------------------------
// setup
GLFWwindow* graph_SetUpRender() {
    static_assert(sizeof(glm::vec3) == sizeof(float) * 3, "is glm stupid?");

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kOpenGLMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kOpenGLMinorVersion);
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

    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwTerminate();
        return nullptr;
    }

    glfwSetFramebufferSizeCallback(window, graph_FrameBufferSizeCallback);
    glfwSetKeyCallback(window, graph_KeyboardCallback);

    glPointSize(kPointSize); $
    glLineWidth(kLineWidth); $

    return window;
}

void graph_TellAboutControls() {
    std::cout << "Quit:      Q" << std::endl
              << "Rotate +X: Z" << std::endl
              << "Rotate +Y: X" << std::endl
              << "Rotate +Z: C" << std::endl
              << "Rotate -X: A" << std::endl
              << "Rotate -Y: S" << std::endl
              << "Rotate -Z: D" << std::endl
              << "Zoom in:   +" << std::endl
              << "Zoom out:  -" << std::endl
              << "Radius+:   1" << std::endl
              << "Radius-:   2" << std::endl;
}

GraphShaders graph_CompileShaders() {
    // load shaders
    std::vector<std::string> file_names_arr;
    file_names_arr.push_back(std::string("./source/graphics/shaders/main_frag_sh.frag"));
    file_names_arr.push_back(std::string("./source/graphics/shaders/main_vert_sh.vert"));

    file_names_arr.push_back(std::string("./source/graphics/shaders/box_frag_sh.frag"));
    file_names_arr.push_back(std::string("./source/graphics/shaders/box_vert_sh.vert"));

    std::vector<std::string> shader_arr = graph_LoadShaders(file_names_arr);
    
    size_t main_frag_ind = 0;
    size_t main_vert_ind = 1;
    size_t box_frag_ind  = 2;
    size_t box_vert_ind  = 3;

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

    glm::mat4 rotate_mat = graph_GetRotationMatrix();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); $
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); $

    // render atoms ------------------------------------------------------------
    GLId VAO_atoms;
    GLId VBO_atoms;
    
    glGenVertexArrays(1, &VAO_atoms); $ // allocate 1 vertex array with id VAO
    glGenBuffers(1, &VBO_atoms); $ // gl allocates 1 buffer with id VBO 
    
    glBindVertexArray(VAO_atoms); $
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_atoms); $ // vbo now is associated with array buffer
    glBufferData(GL_ARRAY_BUFFER, 
                 (GLsizeiptr)(atoms->n_coords * sizeof(glm::vec3)), 
                 atoms->coords, 
                 GL_DYNAMIC_DRAW); $ // gl copy to buffer

    // why 0?
    glVertexAttribPointer(0, kNDimensions, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0); $

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

    // Delete vertex array and buffer for atoms
    glDeleteBuffers(1, &VBO_atoms); $
    glDeleteVertexArrays(1, &VAO_atoms); $
    // render atoms ------------------------------------------------------------

    // render box --------------------------------------------------------------
    std::vector<glm::vec3> box_vec;
    for (size_t i = 0; i < kNOfVertInBox; i++) {
        box_vec.push_back(kBox[i]); 
    }
    graph_CreateCircle(&box_vec);

    GLId VAO_box;
    GLId VBO_box;
    
    glGenVertexArrays(1, &VAO_box); $ // allocate 1 vertex array with id VAO
    glGenBuffers(1, &VBO_box); $ // gl allocates 1 buffer with id VBO 
    
    glBindVertexArray(VAO_box); $
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO_box); $ // vbo now is associated with array buffer

    glBufferData(GL_ARRAY_BUFFER, 
                //  sizeof(kBox),
                 static_cast<GLsizeiptr>(box_vec.size() * sizeof(glm::vec3)), 
                 box_vec.data(), 
                 GL_DYNAMIC_DRAW); $ // gl copy to buffer

    // why 0?
    glVertexAttribPointer(0, kNDimensions, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0); $

    glBindBuffer(GL_ARRAY_BUFFER, 0); $
    glBindVertexArray(0); $

    glUseProgram(shader_ids.box_shader_program_id); $

    int box_rotate_id = glGetUniformLocation(shader_ids.box_shader_program_id, "rotate_mat"); $
    glUniformMatrix4fv(box_rotate_id, 1, GL_FALSE, glm::value_ptr(rotate_mat)); $
    
    int box_scale_id = glad_glGetUniformLocation(shader_ids.box_shader_program_id, "scale_scene");
    glUniform1f(box_scale_id, scale_scene);

    glBindVertexArray(VAO_box); $
        
    glEnableVertexAttribArray(0); $
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(box_vec.size())); $
    glDisableVertexAttribArray(0); $

    glBindBuffer(GL_ARRAY_BUFFER, 0); $
    glBindVertexArray(0); $

    // Delete vertex array and buffer for box
    glDeleteBuffers(1, &VBO_box); $
    glDeleteVertexArrays(1, &VAO_box); $
    // render box --------------------------------------------------------------
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

static glm::mat4 graph_GetRotationMatrix() {
    glm::mat4 rotate_mat_x = glm::mat4(1.0f);
    glm::mat4 rotate_mat_y = glm::mat4(1.0f);
    glm::mat4 rotate_mat_z = glm::mat4(1.0f);

    rotate_mat_x = glm::rotate(rotate_mat_x, glm::radians((float)angle_vec.x), glm::vec3(1.0, 0.0, 0.0));
    rotate_mat_y = glm::rotate(rotate_mat_y, glm::radians((float)angle_vec.y), glm::vec3(0.0, 1.0, 0.0));
    rotate_mat_z = glm::rotate(rotate_mat_z, glm::radians((float)angle_vec.z), glm::vec3(0.0, 0.0, 1.0));

    glm::mat4 rotate_mat = rotate_mat_x * rotate_mat_y * rotate_mat_z;
    return rotate_mat;
} 

static void graph_CreateCircle(std::vector<glm::vec3>* box) {
    assert(box != nullptr);

    float angle_step = 2.0f * std::numbers::pi_v<float> / kNLinesInCircle;

    box->push_back(glm::vec3(-kScale, 0.0f, 1.0f * radius_global));
    for (size_t i = 1; i < kNLinesInCircle; i++) {
        glm::vec3 new_point(-kScale, sin(angle_step * (float)i) * radius_global, cos(angle_step * (float)i) * radius_global);

        box->push_back(new_point);
        box->push_back(new_point);
    }
    box->push_back(glm::vec3(-kScale, 0.0f, 1.0f * radius_global));
}

// callbacks -------------------------------------------------------------------

static void graph_FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    assert(window != nullptr);

    glViewport(0, 0, width, height);
}

static void graph_KeyboardCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {
    switch (key) {
        case GLFW_KEY_Q:
            glfwSetWindowShouldClose(window, GLFW_TRUE); 
            break;
        case GLFW_KEY_Z:
            angle_vec.x++;
            break;
        case GLFW_KEY_X:
            angle_vec.y++;
            break;
        case GLFW_KEY_C:
            angle_vec.z++;
            break;
        case GLFW_KEY_A:
            angle_vec.x--;
            break;
        case GLFW_KEY_S:
            angle_vec.y--;
            break;
        case GLFW_KEY_D:
            angle_vec.z--;
            break;
        case GLFW_KEY_EQUAL:
            scale_scene *= 1.05f;
            break;
        case GLFW_KEY_MINUS:
            scale_scene /= 1.05f;
            break;
        case GLFW_KEY_1:
            radius_global *= 1.05f;
            break;
        case GLFW_KEY_2:
            radius_global /= 1.05f;
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
    std::cerr << "# radius: " << radius << std::endl;
#endif // GRAPH_DEBUG
}
