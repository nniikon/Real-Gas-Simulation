#ifndef GRAPHICS_LOG_H_
#define GRAPHICS_LOG_H_

#include <stddef.h>
#include <stdio.h>

#include "fwd.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "graphics_defs.h"

// debug 
#ifdef GRAPHICS_DEBUG_CALLS
    #define $ GLLogError(__LINE__); fprintf(stderr, "line reached: %d\n", __LINE__);
#else
    #define $ ;
#endif // GRAPHICS_DEBUG_CALLS

void GLLogError(size_t line);
void GLLogShaderError(gl_id shader_id);
void GLLogLinkError(gl_id shader_program);

#endif