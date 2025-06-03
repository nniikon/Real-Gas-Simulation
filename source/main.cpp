#include <iostream>
#include <cstddef>

#include "logs/logs.hpp"
#include "common/gas_structs.hpp"
#include "engine/engine.hpp"
#include "graphics/graphics.hpp"

static const size_t kNOfAtoms = 3'000'000;
float radius_global = 0.1f;

int main(const int argc, const char** argv) {
    GLFWwindow* window = graph_SetUpRender();
    if (window == nullptr) {
        std::cerr << "Unable to open window" << std::endl;
        return -1;
    }

    graph_TellAboutControls();

    GraphShaders shaders = graph_CompileShaders();

    FILE* log_file = logOpenFile("engine_dump.html");
    if (log_file == nullptr) {
        std::cerr << "Error opening logfile" << std::endl;
        return -1;
    }
    eng_SetLogFile(log_file);

    eng_Error eng_error = ENG_ERR_NO;

    gas_Atoms    atom      = {};
    eng_AtomList atom_list = {};

    eng_error = eng_Ctor(&atom, &atom_list, kNOfAtoms);
    if (eng_error != ENG_ERR_NO) {
        std::cerr << "engine ctor error! [ " << __LINE__ << " ]"
                  << std::endl;
        return -1;
    }

    FILE* dump_mes = fopen("mes.txt", "w");
    if (dump_mes == nullptr) {
        std::cerr << "cant open mes.txt file" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        const float dtime = 0.01f; //FIXME
        eng_error = eng_Compute(&atom_list, dtime, radius_global, dump_mes);
        if (eng_error != ENG_ERR_NO) {
            std::cerr << "engine compute error! [ " << __LINE__ << " ]"
                      << std::endl;
            return -1;
        }

        Render(&atom, shaders);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    fclose(log_file);

    glfwTerminate();

    return 0;
}

