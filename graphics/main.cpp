#include "graphics.h"

int main(const int argc, const char** argv) {
    GLFWwindow* window = graph_SetUpRender();
    if (window == nullptr) { return -1; }

    gl_id shader_prog_id = graph_SetUpGl();

    // call atms ctor
    glm::vec3 atms[] = {{-0.5f, -0.5f, 0.0f},
                        { 0.5f, -0.5f, 0.0f},
                        { 0.0f,  0.5f, 0.0f}};

    size_t n_at = 3;
    gas_Atoms atoms = {
        .coords = atms,
        .n_coords = n_at,
        .radius = 1.0f
    };

    while (!glfwWindowShouldClose(window)) {    
        // call compute
        Render(&atoms, shader_prog_id);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}