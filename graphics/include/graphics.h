#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <cassert>

#include "../../glad/include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "../../common/gas_structs.h"

void graph_GLFWInit();
void graph_DoMagic(GLFWwindow* window, gas_Atoms* atoms);

#endif // GRAPHICS_H_