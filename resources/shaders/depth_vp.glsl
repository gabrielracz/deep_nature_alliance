#version 330 core

layout (location = 0) in vec3 vertex;

uniform mat4 world_mat;
uniform mat4 light_mat;

void main()
{
    gl_Position = light_mat * world_mat * vec4(vertex, 1.0);
}  