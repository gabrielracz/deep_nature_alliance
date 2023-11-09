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
uniform mat4 normal_mat;
uniform vec3 light_pos_world;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec3 normal_interp;
out vec4 color_interp;
out vec2 uv_interp;
out vec3 light_pos;


void main()
{
    position_interp = vec3(view_mat * world_mat * vec4(vertex, 1.0));
    gl_Position = projection_mat * vec4(position_interp, 1.0f);
    normal_interp = vec3(normal_mat * vec4(normal, 0.0f));
    color_interp = vec4(0.7, 0.7, 0.8, 1.0f);
    // color_interp.a = 1.0f;
    // color_interp = vec4(color, 1.0f);
    uv_interp = uv;
    vec3 light_pos_w = vec3(0, 0, 0);
    light_pos = vec3(view_mat * vec4(light_pos_w, 1.0));
}
