#include "graphics_log.h"

// debug -----------------------------------------------------------------------
__attribute__((unused))
void GLLogError(const char* file_name, size_t line) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "! Opengl error: %u, %s:%lu\n", error, file_name, line);
    }
}

void GLLogShaderError(GLId shader_id) {
#ifdef GRAPHICS_DEBUG_SHADER
    int success = 0;
    char log[512];
    
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success); $
    if (!success) {
        glGetShaderInfoLog(shader_id, sizeof(log), NULL, log); $
        fprintf(stderr, "# gl error: shader: %s\n", log); $
    }
#endif // GRAPHICS_DEBUG
}

void GLLogLinkError(GLId shader_program) {
#ifdef GRAPHICS_DEBUG_SHADER
    int success = 0;
    char log[512];
    
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success); $
    if (!success) {
        glGetProgramInfoLog(shader_program, sizeof(log), NULL, log); $
        fprintf(stderr, "# gl error: shader program: %s\n", log); $
    }
#endif // GRAPHICS_DEBUG
}
