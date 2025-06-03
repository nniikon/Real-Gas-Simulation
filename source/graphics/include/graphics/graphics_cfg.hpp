#ifndef GRAPHICS_CFG_H_
#define GRAPHICS_CFG_H_

#include <cstddef>

#if defined (GRAPH_DEBUG)
    #define GRAPHICS_DEBUG_SHADER
    #define GRAPHICS_DEBUG_CALLS
#endif // DEBUG

#define UNUSED __attribute__((unused))

UNUSED static const int kOpenGLMajorVersion    = 3;
UNUSED static const int kOpenGLMinorVersion    = 3;

UNUSED static const size_t kNDimensions        = 3;
UNUSED static const int    graph_kWindowWidth  = 1024;
UNUSED static const int    graph_kWindowHeight = 1024;
UNUSED static const char*  graph_kWindowTitle  = "gas experiment";
UNUSED static const float  kPointSize          = 2.0f;
UNUSED static const float  kLineWidth          = 2.5f;
UNUSED static const size_t kNLinesInCircle     = 32;

#endif // GRAPHICS_CFG_H_
