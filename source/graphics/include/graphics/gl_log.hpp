#ifndef GL_LOG_HPP_
#define GL_LOG_HPP_

#include <cstddef>
#include <cstdio>
#include <source_location>

#include <glad/gl.h>

#include "graphics/config.hpp"

namespace gas {
namespace grx {

inline void GlDbg([[maybe_unused]] std::source_location src_loc = std::source_location::current()) {
#if defined (GRAPHICS_DEBUG_CALLS)
    GLenum error = 0; 
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "! Opengl error: %u, %s:%u\n", error, src_loc.file_name(), src_loc.line());
    }
#endif // GRAPHICS_DEBUG_CALLS
}

} // namespace grx
} // namespace gas

#endif // GL_LOG_HPP_
