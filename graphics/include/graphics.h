#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <assert.h>
#include <stdio.h>

#include "fwd.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "gas_structs.h"
#include "graphics_cfg.h"

typedef unsigned int gl_id;

// setup
GLFWwindow* graph_SetUpRender();
// returns shader program handle
gl_id graph_SetUpGl();

void Render(gas_Atoms* atoms, gl_id shader_prog_id);

#endif // GRAPHICS_H_