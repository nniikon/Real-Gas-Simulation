#include "gas_structs.h"
#include "graphics.h"
#include "engine.h"
#include <cstddef>

static const size_t kNOfAtoms = 10000;

int main(const int argc, const char** argv) {
    GLFWwindow* window = graph_SetUpRender();
    if (window == nullptr) { return -1; }

    gl_id shader_prog_id = graph_SetUpGl();

    eng_Error eng_error = ENG_ERR_NO;

    gas_Atoms    atom      = {};
    eng_AtomList atom_list = {};

    eng_error = eng_Ctor(&atom, &atom_list, kNOfAtoms);
    if (eng_error != ENG_ERR_NO) { fprintf(stderr, "fuck! [ %d ]\n", __LINE__); }

    while (!glfwWindowShouldClose(window)) {
        eng_error = eng_Compute(&atom_list, 0.00001);
        if (eng_error != ENG_ERR_NO) { fprintf(stderr, "fuck! [ %d ]\n", __LINE__); }        
        Render(&atom, shader_prog_id);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    glfwTerminate();

    return 0;
}