#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <assert.h>
#include <stdio.h>

#include "fwd.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "gas_structs.h"
#include "graphics_cfg.h"
#include "graphics_defs.h"

// setup
GLFWwindow* graph_SetUpRender();
// returns shader program handle
GLId graph_CompileShaders();

void Render(gas_Atoms* atoms, GLId shader_prog_id);

#endif // GRAPHICS_H_
