#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <cstddef>
#include <string_view>

namespace gas {
namespace grx {

// #define GRAPH_DEBUG
#if defined (GRAPH_DEBUG)
    #define GRAPHICS_DEBUG_SHADER
    #define GRAPHICS_DEBUG_CALLS
#endif // DEBUG

constexpr inline int kOpenGLMajorVersion = 3;
constexpr inline int kOpenGLMinorVersion = 3;

constexpr inline size_t kNDimensions = 3;
constexpr inline int kWindowWidth = 1024;
constexpr inline int kWindowHeight = 1024;
constexpr inline 
    std::string_view kWindowTitle = "gas experiment";

constexpr inline float kPointSize = 2.0f;
constexpr inline float kLineWidth = 2.5f;
constexpr inline size_t kNLinesInCircle = 32;

} // namespace grx
} // namespace gas

#endif // CONFIG_HPP_
