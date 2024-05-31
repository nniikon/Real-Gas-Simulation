#include "gas_structs.h"
#include "graphics.h"
#include "engine.h"
#include "../../libs/logs/logs.h"
#include <cstddef>
#include <unistd.h>

static const size_t kNOfAtoms = 05000;

int main(const int argc, const char** argv) {
    GLFWwindow* window = graph_SetUpRender();
    if (window == nullptr) { return -1; }

    gl_id shader_prog_id = graph_SetUpGl();

    FILE* log_file = logOpenFile("VOVA_LOH.html");
    if (log_file == nullptr) {
        fprintf(stderr, "Error opening logfile\n");
        return 1;
    }
    eng_SetLogFile(log_file);

    eng_Error eng_error = ENG_ERR_NO;

    gas_Atoms    atom      = {};
    eng_AtomList atom_list = {};

    eng_error = eng_Ctor(&atom, &atom_list, kNOfAtoms);
    if (eng_error != ENG_ERR_NO) { fprintf(stderr, "fuck! [ %d ]\n", __LINE__); }

    while (!glfwWindowShouldClose(window)) {
        eng_error = eng_Compute(&atom_list, 0.01f);
        if (eng_error != ENG_ERR_NO) { fprintf(stderr, "fuck! [ %d ]\n", __LINE__); }        
        Render(&atom, shader_prog_id);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

        //sleep(1);
    }

    fclose(log_file);

    glfwTerminate();

    return 0;
}
