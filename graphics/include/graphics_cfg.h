#ifndef GRAPHICS_CFG_H_
#define GRAPHICS_CFG_H_

#include <stddef.h>

#if defined (DEBUG)
    #define GRAPHICS_DEBUG_SHADER
    #define GRAPHICS_DEBUG_CALLS
#endif // DEBUG

static const int   graph_kWindowWidth = 1024;
static const int   graph_kWindowHeight = 512;
__attribute__((unused))
static const char* graph_kWindowTitle = "real shit";
__attribute__((unused))
static const size_t kNDimensions = 3; 

#endif // GRAPHICS_CFG_H_