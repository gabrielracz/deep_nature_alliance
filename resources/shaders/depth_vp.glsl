#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 light_view_matrix;
uniform mat4 light_projection_matrix;

void main()
{
    gl_Position = light_projection_matrix * light_view_matrix * vec4(aPos, 1.0);
}  