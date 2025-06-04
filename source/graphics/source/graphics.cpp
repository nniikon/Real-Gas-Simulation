#include "graphics/graphics.hpp"

#include <GLFW/glfw3.h>
#include <cstddef>
#include <string>
#include <vector>
#include <iostream>
#include <numbers>
#include <memory>
#include <stdexcept>

#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glad/gl.h>

#include "common/gas_structs.hpp"

#include "graphics/graphics_cfg.hpp"
#include "graphics/graphics_defs.hpp"
#include "graphics/graphics_log.hpp"
#include "graphics/graphics_shaders.hpp"
#include "graphics/graphics_box.hpp"

extern float radius_global;

namespace gas {
namespace grx {

// static ----------------------------------------------------------------------

namespace {

glm::ivec3 angle_vec(0, 0, 0);
float scale_scene = 0.5f;

// callbacks
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
void KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
glm::mat4 GetRotationMatrix();
GLId CompileProgram(const std::string& vert_shader, const std::string& frag_shader);
void CreateCircle(std::vector<glm::vec3>* box);

} // namespace 

// global ----------------------------------------------------------------------

// setup
WindowHandle SetUpRender() {
    static_assert(sizeof(glm::vec3) == sizeof(float) * 3, "is glm stupid?");

    auto window = glfwCreateWindow(
        kWindowHeight,                           
        kWindowHeight,
        kWindowTitle.data(),
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        throw std::runtime_error{"cant open window"};
    }

    WindowHandle wnd_handle{window, &WindowDeleter};

    glfwMakeContextCurrent(wnd_handle.get());

    if (!gladLoadGL(glfwGetProcAddress)) {
        throw std::runtime_error{"cant load OpenGL"};
    }

    glfwSetFramebufferSizeCallback(wnd_handle.get(), FrameBufferSizeCallback);
    glfwSetKeyCallback(wnd_handle.get(), KeyboardCallback);

    glPointSize(kPointSize); GlDbg();
    glLineWidth(kLineWidth); GlDbg();

    return wnd_handle;
}

std::string TellAboutControls() {
    std::string controls = 
        "Quit:      Q\n"
        "Rotate +X: Z\n"
        "Rotate +Y: X\n"
        "Rotate +Z: C\n"
        "Rotate -X: A\n"
        "Rotate -Y: S\n"
        "Rotate -Z: D\n"
        "Zoom in:   +\n"
        "Zoom out:  -\n"
        "Radius+:   1\n"
        "Radius-:   2\n";
    return controls;
}

GraphShaders CompileShaders() {
    // load shaders
    std::vector<std::string> file_names_arr;
    file_names_arr.emplace_back("./source/graphics/shaders/main_frag_sh.frag");
    file_names_arr.emplace_back("./source/graphics/shaders/main_vert_sh.vert");

    file_names_arr.emplace_back("./source/graphics/shaders/box_frag_sh.frag");
    file_names_arr.emplace_back("./source/graphics/shaders/box_vert_sh.vert");

    std::vector<std::string> shader_arr = LoadShaders(file_names_arr);

    constexpr size_t main_frag_ind = 0;
    constexpr size_t main_vert_ind = 1;
    constexpr size_t box_frag_ind  = 2;
    constexpr size_t box_vert_ind  = 3;

    auto&& main_frag_shader = shader_arr[main_frag_ind];
    auto&& main_vert_shader = shader_arr[main_vert_ind];
    auto&& box_frag_shader  = shader_arr[box_frag_ind];
    auto&& box_vert_shader  = shader_arr[box_vert_ind];

    GLId main_shader_program_id = CompileProgram(main_vert_shader, main_frag_shader);
    GLId box_shader_program_id = CompileProgram(box_vert_shader, box_frag_shader);

    return GraphShaders{main_shader_program_id, box_shader_program_id};
}

// render
void Render(gas_Atoms* atoms, const GraphShaders& shader_ids) {
    assert(atoms != nullptr);

    glm::mat4 rotate_mat = GetRotationMatrix();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GlDbg();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GlDbg();

    // render atoms ------------------------------------------------------------
    GLId VAO_atoms{};
    GLId VBO_atoms{};

    glGenVertexArrays(1, &VAO_atoms); GlDbg(); // allocate 1 vertex array with id VAO
    glGenBuffers(1, &VBO_atoms); GlDbg(); // gl allocates 1 buffer with id VBO

    glBindVertexArray(VAO_atoms); GlDbg();

    glBindBuffer(GL_ARRAY_BUFFER, VBO_atoms); GlDbg(); // vbo now is associated with array buffer
    glBufferData(
        GL_ARRAY_BUFFER,
        (GLsizeiptr)(atoms->n_coords * sizeof(glm::vec3)),
        atoms->coords,
        GL_DYNAMIC_DRAW
    ); GlDbg(); // gl copy to buffer

    // why 0?
    glVertexAttribPointer(
        0, 
        kNDimensions, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(glm::vec3), 
        nullptr
    ); GlDbg();

    glBindBuffer(GL_ARRAY_BUFFER, 0); GlDbg();
    glBindVertexArray(0); GlDbg();

    glUseProgram(shader_ids.main_shader_program_id); GlDbg();

    int main_rotate_id = glGetUniformLocation(
        shader_ids.main_shader_program_id, 
        "rotate_mat"
    ); GlDbg();
    glUniformMatrix4fv(
        main_rotate_id, 
        1, 
        GL_FALSE, 
        glm::value_ptr(rotate_mat)
    ); GlDbg();

    int main_scale_id = glGetUniformLocation(
        shader_ids.main_shader_program_id, 
        "scale_scene"
    ); GlDbg();
    glUniform1f(main_scale_id, scale_scene); GlDbg();

    glBindVertexArray(VAO_atoms); GlDbg();

    glEnableVertexAttribArray(0); GlDbg();
    glDrawArrays(GL_POINTS, 0, (GLsizei)atoms->n_coords); GlDbg();
    glDisableVertexAttribArray(0); GlDbg();

    glBindBuffer(GL_ARRAY_BUFFER, 0); GlDbg();
    glBindVertexArray(0); GlDbg();

    // Delete vertex array and buffer for atoms
    glDeleteBuffers(1, &VBO_atoms); GlDbg();
    glDeleteVertexArrays(1, &VAO_atoms); GlDbg();
    // render atoms ------------------------------------------------------------

    // render box --------------------------------------------------------------
    std::vector<glm::vec3> box_vec;
    box_vec.reserve(kNOfVertInBox + kNLinesInCircle * 2);
    for (size_t i = 0; i < kNOfVertInBox; i++) {
        box_vec.push_back(kBox[i]);
    }
    CreateCircle(&box_vec);

    GLId VAO_box;
    GLId VBO_box;

    glGenVertexArrays(1, &VAO_box); GlDbg(); // allocate 1 vertex array with id VAO
    glGenBuffers(1, &VBO_box); GlDbg(); // gl allocates 1 buffer with id VBO

    glBindVertexArray(VAO_box); GlDbg();

    glBindBuffer(GL_ARRAY_BUFFER, VBO_box); GlDbg(); // vbo now is associated with array buffer

    glBufferData(
        GL_ARRAY_BUFFER,
        //  sizeof(kBox),
        static_cast<GLsizeiptr>(box_vec.size() * sizeof(glm::vec3)),
        box_vec.data(),
        GL_DYNAMIC_DRAW
    ); GlDbg(); // gl copy to buffer

    // why 0?
    glVertexAttribPointer(
        0, 
        kNDimensions, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(glm::vec3), 
        nullptr
    ); GlDbg();

    glBindBuffer(GL_ARRAY_BUFFER, 0); GlDbg();
    glBindVertexArray(0); GlDbg();

    glUseProgram(shader_ids.box_shader_program_id); GlDbg();

    int box_rotate_id = glGetUniformLocation(
        shader_ids.box_shader_program_id, 
        "rotate_mat"
    ); GlDbg();
    glUniformMatrix4fv(box_rotate_id, 1, GL_FALSE, glm::value_ptr(rotate_mat)); GlDbg();

    int box_scale_id = glad_glGetUniformLocation(
        shader_ids.box_shader_program_id, 
        "scale_scene"
    ); GlDbg();
    glUniform1f(box_scale_id, scale_scene); GlDbg();

    glBindVertexArray(VAO_box); GlDbg();

    glEnableVertexAttribArray(0); GlDbg();
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(box_vec.size())); GlDbg();
    glDisableVertexAttribArray(0); GlDbg();

    glBindBuffer(GL_ARRAY_BUFFER, 0); GlDbg();
    glBindVertexArray(0); GlDbg();

    // Delete vertex array and buffer for box
    glDeleteBuffers(1, &VBO_box); GlDbg();
    glDeleteVertexArrays(1, &VAO_box); GlDbg();
    // render box --------------------------------------------------------------
}

// static ----------------------------------------------------------------------

namespace {

GLId CompileProgram(const std::string& vert_shader, const std::string& frag_shader) {
    auto&& vert_cstr = vert_shader.c_str();
    auto&& frag_cstr = frag_shader.c_str();

    GLId vertex_shader_id = glCreateShader(GL_VERTEX_SHADER); GlDbg();
    glShaderSource(vertex_shader_id, 1, &vert_cstr, NULL); GlDbg();
    glCompileShader(vertex_shader_id); GlDbg();
    GLLogShaderError(vertex_shader_id); GlDbg();

    GLId fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER); GlDbg();
    // 1 is the number of strings of source code,
    // NULL means that strings are null terminated
    glShaderSource(fragment_shader_id, 1, &frag_cstr, NULL); GlDbg();
    glCompileShader(fragment_shader_id); GlDbg();
    GLLogShaderError(fragment_shader_id); GlDbg();

    GLId shader_prog_id = glCreateProgram(); GlDbg();
    glAttachShader(shader_prog_id, vertex_shader_id); GlDbg();
    glAttachShader(shader_prog_id, fragment_shader_id); GlDbg();
    glLinkProgram(shader_prog_id); GlDbg();
    GLLogLinkError(shader_prog_id); GlDbg();

    glDeleteShader(vertex_shader_id); GlDbg();
    glDeleteShader(fragment_shader_id); GlDbg();

    return shader_prog_id;
}

glm::mat4 GetRotationMatrix() {
    glm::mat4 rotate_mat_x = glm::mat4(1.0f);
    glm::mat4 rotate_mat_y = glm::mat4(1.0f);
    glm::mat4 rotate_mat_z = glm::mat4(1.0f);

    rotate_mat_x = glm::rotate(rotate_mat_x, glm::radians((float)angle_vec.x), glm::vec3(1.0, 0.0, 0.0));
    rotate_mat_y = glm::rotate(rotate_mat_y, glm::radians((float)angle_vec.y), glm::vec3(0.0, 1.0, 0.0));
    rotate_mat_z = glm::rotate(rotate_mat_z, glm::radians((float)angle_vec.z), glm::vec3(0.0, 0.0, 1.0));

    glm::mat4 rotate_mat = rotate_mat_x * rotate_mat_y * rotate_mat_z;
    return rotate_mat;
}

void CreateCircle(std::vector<glm::vec3>* box) {
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

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    assert(window != nullptr);

    glViewport(0, 0, width, height);
}

void KeyboardCallback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {
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
    std::cerr << "# radius: " << radius_global << std::endl;
#endif // GRAPH_DEBUG
}

} // namespace

} // namespace grx
} // namespace gas
