// Source code of vertex shader
#version 330

// Vertex buffer
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;

// Attributes forwarded to the fragment shader
out vec2 uv_interp;

void main()
{
    // Transform vertex
	vec4 vertex_pos = vec4(vertex.xy,0.0, 1.0);
	gl_Position = world_mat * projection_mat* vertex_pos;
//	gl_Position = view_matrix * vertex_pos;

    // Pass attributes to fragment shader
    //color_interp = vec4(color, 1.0);
    // uv_interp = uv;
    uv_interp = vec2(uv.x, 1.0 - uv.y);
}
