#include <iostream>
#include <cstddef>
#include <stdexcept>

#include "logs/logs.hpp"

#include "common/gas_structs.hpp"

#include "engine/engine.hpp"

#include "graphics/graphics.hpp"
#include "graphics/gl_log.hpp"

static constexpr size_t kNOfAtoms = 30'000;
float radius_global = 0.1f;

int main(const int argc, const char** argv) {
    gas::grx::GraphicsContext cnt{};

    gas::grx::WindowHandle wnd_handle{nullptr, gas::grx::WindowDeleter};

    try {
        wnd_handle = gas::grx::SetUpRender();
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl; 
        return -1;
    }

    std::cout << gas::grx::TellAboutControls();

    auto main_program = gas::grx::GetMainShaderProgram();
    auto box_program = gas::grx::GetBoxShaderProgram();

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

    while (!glfwWindowShouldClose(wnd_handle.get())) {
        const float dtime = 0.01f; //FIXME
        eng_error = eng_Compute(&atom_list, dtime, radius_global, dump_mes);
        if (eng_error != ENG_ERR_NO) {
            std::cerr << "engine compute error! [ " << __LINE__ << " ]"
                      << std::endl;
            return -1;
        }

        // frame prologue
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); gas::grx::GlDbg();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); gas::grx::GlDbg();

        gas::grx::Render(&atom, main_program, box_program);

        // frame epilogue
        glfwSwapBuffers(wnd_handle.get());
        glfwPollEvents();
    }

    fclose(log_file);

    return 0;
}

