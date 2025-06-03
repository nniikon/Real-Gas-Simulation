#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <assert.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "common/gas_structs.hpp"

#include "graphics/graphics_cfg.hpp"
#include "graphics/graphics_defs.hpp"

struct GraphShaders {
    GLId main_shader_program_id;
    GLId box_shader_program_id;
};

// setup
GLFWwindow* graph_SetUpRender();
// control dump
void graph_TellAboutControls();
// returns shader program handle
GraphShaders graph_CompileShaders();

void Render(gas_Atoms* atoms, const GraphShaders& shader_ids);

#endif // GRAPHICS_H_
