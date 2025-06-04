#ifndef GRAPHICS_LOG_HPP_
#define GRAPHICS_LOG_HPP_

#include <cstddef>
#include <cstdio>
#include <source_location>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "graphics/graphics_defs.hpp"
#include "graphics/graphics_cfg.hpp"

#define $ gas::grx::GlDbg();

namespace gas {
namespace grx {

void GLLogError(const char* file_name, size_t line);
void GLLogShaderError(GLId shader_id);
void GLLogLinkError(GLId shader_program);

inline void GlDbg([[maybe_unused]] std::source_location src_loc = std::source_location::current()) {
#if defined (GRAPHICS_DEBUG_CALLS)
    GLLogError(src_loc.file_name(), src_loc.line());
#endif // GRAPHICS_DEBUG_CALLS
}

} // namespace grx
} // namespace gas

#endif // GRAPHICS_LOG_HPP_
