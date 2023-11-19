#version 330

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

struct Light {
    vec3 position;
    float ambient_strength;
    vec4 color;
    vec4 ambient_color;
    vec3 direction;
    float spread;
};

out Light lights[6];
flat out int num_lights;

layout(std140) uniform LightsBlock {
    Light world_lights[6];
};
uniform int num_world_lights;


void main()
{
    position_interp = vec3(view_mat * world_mat * vec4(vertex, 1.0));
    gl_Position = projection_mat * vec4(position_interp, 1.0f);

    normal_interp = vec3(normal_mat * vec4(normal, 0.0));
    color_interp = vec4(color, 1.0);
    uv_interp = uv;

    // light_pos = vec3(view_mat * vec4(light_pos_world, 1.0));

    for(int i = 0; i < num_world_lights; i++) {
        lights[i].position         = vec3(view_mat * vec4(world_lights[i].position, 1.0));
        lights[i].color            = world_lights[i].color;
        lights[i].ambient_color    = world_lights[i].ambient_color;
        lights[i].ambient_strength = world_lights[i].ambient_strength;
    }
    num_lights = num_world_lights;
}
