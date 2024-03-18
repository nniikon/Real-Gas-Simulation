#include "include/graphics.h"
// #include <GLFW/glfw3.h>

const int graph_kWindowWidth = 1024;
const int graph_kWindowHight = 512;
const char* graph_kWindowTitle = "real shit";

int main(const int argc, const char** argv) {
    graph_GLFWInit();

    GLFWwindow* window = glfwCreateWindow(graph_kWindowHight,
                                         graph_kWindowHight,
                                          graph_kWindowTitle,
                                        NULL,
                                          NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // call set_calc

    gas_Atoms* atoms = NULL;
    graph_DoMagic(window, atoms);

    glfwDestroyWindow(window);

    return 0;
}