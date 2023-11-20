#version 130
#pragma optionNV(unroll all)

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;

// Attributes forwarded to the fragment shader
out vec3 position_interp;
out vec3 normal_interp;
out vec4 color_interp;
out vec2 uv_interp;
out vec3 light_pos;

// Material attributes (constants)
uniform vec3 light_position = vec3(50.5, -0.5, -5005.5);


void main()
{
   // vec3 vertexoffset = vec3(gl_InstanceID, gl_InstanceID*0.2, -gl_InstanceID*3);
	
	float off1 = ((gl_InstanceID*8721221+4322)%7979)/7979.0 - 0.5;
	float off2 = ((gl_InstanceID*3421217+4343)%14179)/14179.0 - 0.5;
	float off3 = ((gl_InstanceID*2821291+54332)%37291)/37921.0 - 0.5;
    vec3 vertexoffset = vec3(off1*2,off2*2,off3*2);

	gl_Position = projection_mat * view_mat * (world_mat * vec4(vertex, 1.0) + vec4(vertexoffset,1.0));

    position_interp = vec3(view_mat * world_mat * vec4(vertex+vertexoffset, 1.0));
    
   normal_interp = vec3(normal_mat * vec4(normal, 0.0));

    color_interp = vec4(color, 1.0);

    uv_interp = uv;

    light_pos = vec3(view_mat * vec4(light_position, 1.0));
}
