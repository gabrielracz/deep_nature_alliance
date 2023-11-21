#version 330 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec2 uv;
layout (location = 4) in vec3 tangent;

// Uniform (global) buffer

uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec2 uv_interp;
out vec3 light_pos;
out vec3 color_interp;
out vec3 normal_interp;

out float test;

struct Light {
    vec3 position;
    float ambient_strength;
    vec4 color;
    vec4 ambient_color;
    vec3 direction;
    float spread;
};

out Light lights[3];
flat out int num_lights;

layout(std140) uniform LightsBlock {
    Light world_lights[3];
};
uniform int num_world_lights;

uniform int num_instances;
layout(std140) uniform TransformsBlock {
    mat4 transformations[2048];
};

void main()
{
    vec4 position = view_mat * world_mat * transformations[gl_InstanceID] * vec4(vertex, 1.0);
    gl_Position = projection_mat * position;

    // Define vertex tangent, bitangent and normal (TBN)
    // These are used to create the tangent space transformation matrix
    vec3 vertex_normal = vec3(normal_mat * vec4(normal, 0.0));
    vec3 vertex_tangent_ts = vec3(normal_mat * vec4(tangent, 0.0));
    vec3 vertex_bitangent_ts = cross(vertex_normal, vertex_tangent_ts);

    // Send tangent space transformation matrix to the fragment shader
    mat3 TBN_mat = transpose(mat3(vertex_tangent_ts, vertex_bitangent_ts, vertex_normal));

    position_interp = TBN_mat * vec3(position);
    normal_interp = TBN_mat * vertex_normal;
    for(int i = 0; i < num_world_lights; i++) {
        lights[i].position         = TBN_mat * vec3(view_mat * vec4(world_lights[i].position, 1.0));
        lights[i].color            = world_lights[i].color;
        lights[i].ambient_color    = world_lights[i].ambient_color;
        lights[i].ambient_strength = world_lights[i].ambient_strength;
    }
    num_lights = num_world_lights;


    color_interp = color;
    uv_interp = uv; 
}
