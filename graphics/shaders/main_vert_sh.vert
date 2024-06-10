#version 330 core

layout (location = 0) in vec3 aPos;

uniform float scale_scene;
uniform mat4 rotate_mat;

out vec4 vert_pos;

void main() {
    gl_Position = rotate_mat * vec4(scale_scene * aPos.x, scale_scene * aPos.y, scale_scene * aPos.z, 1.0);
    
    vert_pos = gl_Position;
}