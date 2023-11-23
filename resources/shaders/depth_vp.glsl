#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 light_space_matrix;

void main()
{
    // gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    gl_Position = light_space_matrix * vec4(aPos, 1.0);
}  