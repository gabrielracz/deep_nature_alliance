#version 330

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
uniform vec3 light_pos_world;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec2 uv_interp;
out vec3 light_pos;
out vec3 color_interp;
out vec3 normal_interp;

// Material attributes (constants)
uniform vec3 light_position = vec3(-0.5, -0.5, 1.5);


void main()
{
    vec4 position = view_mat * world_mat * vec4(vertex, 1.0);
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
    light_pos = TBN_mat * vec3(view_mat * vec4(light_pos_world, 1.0));

    color_interp = color;
    uv_interp = uv; 
}
