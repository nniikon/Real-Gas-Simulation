#ifndef GRAPHICS_SHADERS_H_
#define GRAPHICS_SHADERS_H_

static const char* vertex_shader = "#version 330 core\n"
                                    "layout (location = 0) in vec3 aPos;\n"
                                    "void main() {\n"
                                    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                    "}\n\0";

static const char* fragment_shader = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "void main() {\n"
                                        "FragColor = vec4(1.0, 0.5, 0.5, 1.0f);\n"
                                    "}\n\0";

#endif // GRAPHICS_SHADERS_H_