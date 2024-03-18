#include "../include/graphics.h"
// static ---------------------------------------------------------------------


static void Render(gas_Atoms* atoms);

// global ---------------------------------------------------------------------

void graph_GLFWInit() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void graph_DoMagic(GLFWwindow* window, gas_Atoms* atoms) {
    assert(window != nullptr);
    // assert(atoms != nullptr);

    while (!glfwWindowShouldClose(window)) {
        Render(atoms);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

void Render(gas_Atoms* atoms) {
//   assert(atoms != nullptr);

    // for (size_t i = 0; i < atoms->n_coords; i++) {

    // }

    // glClear(GL_COLOR_BUFFER_BIT);
    // glBegin(GL_POINTS);
}