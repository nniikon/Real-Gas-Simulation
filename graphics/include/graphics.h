#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <assert.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "gas_structs.h"

#include "graphics_cfg.h"
#include "graphics_defs.h"

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
