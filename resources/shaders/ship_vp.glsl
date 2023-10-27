// Material with no illumination simulation

#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;

// in vec3 vertex;
// in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

// Attributes forwarded to the fragment shader
out vec4 color_interp;
out vec4 normal_interp;


void main()
{
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
    normal_interp = world_mat * vec4(normal, 0.0f);

    color_interp = vec4(normal.z, normal.y, normal.x, 1.0);
}
