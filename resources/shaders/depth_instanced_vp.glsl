#version 330 core

layout (location = 0) in vec3 vertex;

uniform mat4 world_mat;
uniform mat4 light_mat;

struct Instance {
    mat4 transformation;
    mat4 normal_matrix;
};

uniform int num_instances;
layout(std140) uniform TransformsBlock {
    Instance instances[512];
};

void main()
{
    gl_Position = light_mat * world_mat * instances[gl_InstanceID].transformation * vec4(vertex, 1.0);
}  