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
out vec3 position_interp;
out vec3 normal_interp;
out vec4 color_interp;


void main()
{
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
    normal_interp = vec3(world_mat * vec4(normal, 0.0f));
    // color_interp = vec4(normal_interp.z, normal_interp.y, normal_interp.x, 1.0);
    // color_interp = vec4(0x67/255.0, 0x5c/255.0f, 1.0, 1.0);
    color_interp = vec4(0.7, 0.7, 0.8, 1.0f);
}
