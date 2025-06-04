#include "graphics/graphics_log.hpp"

#include <cassert>

#include "graphics/graphics_cfg.hpp"

namespace gas {
namespace grx {

// debug -----------------------------------------------------------------------
[[maybe_unused]]
void GLLogError(const char* file_name, size_t line) {
    assert(file_name != nullptr);

    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "! Opengl error: %u, %s:%lu\n", error, file_name, line);
    }
}

void GLLogShaderError([[maybe_unused]] GLId shader_id) {
#if defined (GRAPHICS_DEBUG_SHADER)
    int success = 0;
    static char log[512];

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success); $
    if (!success) {
        glGetShaderInfoLog(shader_id, sizeof(log), NULL, log); $
        fprintf(stderr, "# gl error: shader: %s\n", log); $
    }
#endif // GRAPHICS_DEBUG_SHADER
}

void GLLogLinkError([[maybe_unused]] GLId shader_program) {
#ifdef GRAPHICS_DEBUG_SHADER
    int success = 0;
    static char log[512];

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success); $
    if (!success) {
        glGetProgramInfoLog(shader_program, sizeof(log), NULL, log); $
        fprintf(stderr, "# gl error: shader program: %s\n", log); $
    }
#endif // GRAPHICS_DEBUG
}

} // namespace grx
} // namespace gas
