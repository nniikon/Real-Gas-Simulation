#ifndef GRAPHICS_LOG_H_
#define GRAPHICS_LOG_H_

#include <stddef.h>
#include <stdio.h>

#include "glm/fwd.hpp"
#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "graphics_defs.h"

// debug 
#ifdef GRAPHICS_DEBUG_CALLS
    #define $ GLLogError(__FILE__, __LINE__); 
#else
    #define $ ;
#endif // GRAPHICS_DEBUG_CALLS

void GLLogError(const char* file_name, size_t line);
void GLLogShaderError(GLId shader_id);
void GLLogLinkError(GLId shader_program);

#endif
