#include "gas_structs.h"
#include "graphics.h"
#include "engine.h"
#include "../../libs/logs/logs.h"
#include <cstddef>
#include <unistd.h>

static const size_t kNOfAtoms = 700;

int main(const int argc, const char** argv) {
    GLFWwindow* window = graph_SetUpRender();
    if (window == nullptr) { return -1; }

    graph_TellAboutControls();

    GraphShaders shaders = graph_CompileShaders();
    glPointSize(3.0f); //FIXME

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
    if (eng_error != ENG_ERR_NO) { fprintf(stderr, "engine ctor error! [ %d ]\n", __LINE__); return 1; }

    while (!glfwWindowShouldClose(window)) {
        eng_error = eng_Compute(&atom_list, 0.01f);
        if (eng_error != ENG_ERR_NO) { fprintf(stderr, "engine compute error! [ %d ]\n", __LINE__); return 1; }        

        Render(&atom, shaders);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    fclose(log_file);

    glfwTerminate();

    return 0;
}
