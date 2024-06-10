#version 330 core
in vec4 vert_pos; // Position of the vertex in clip space

out vec4 FragColor;

void main()
{
    // Calculate distance from camera (in clip space, w is the depth value)
    float distance = vert_pos.z / vert_pos.w;

    // Normalize distance to [0, 1] range (you may need to adjust this based on your scene)
    distance = (distance + 1.0) / 2.0;

    // Calculate color intensity (closer points are brighter, further points are darker)
    float intensity = distance;
    
    mat4 depth_mat = mat4(intensity);

    // Set the fragment color
    FragColor = depth_mat * vec4(1.0f, 0.5f, 0.5f, 1.0f);
}
