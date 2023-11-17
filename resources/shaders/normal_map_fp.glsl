#version 330

// Attributes passed from the vertex shader
in vec3 color_interp;
in vec3 vertex_position;
in vec2 vertex_uv;
in vec3 normal_interp;
// in mat3 TBN_mat;
in vec3 light_pos;

layout(location = 0) out vec4 fragcolor;

// Uniform (global) buffer
// layout(binding=0) uniform sampler2D texture_map;
// layout(binding=1) uniform sampler2D normal_map; // Normal map

uniform sampler2D texture_map;
uniform sampler2D normal_map; // Normal map

// Material attributes (constants)
// uniform vec4 object_color = vec4(0.79, 0.96, 0.60, 1.0);


void main() 
{

// Incomplete demo -- does matrix multiplication in fragment shader.
// Left as exercise: move matrix multiplication to vertex shader,
// use interpolated L, V in fragment shader

    // Blinn-Phong shading
    // vec4 tex_color = texture2D(texture_map, vertex_uv);
    // vec4 object_color = vec4(tex_color.xyz * color_interp.xyz, 1.0f);
    vec4 object_color = texture(texture_map, vertex_uv);
    // vec4 object_color = vec4(color_interp, 1.0);

    vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector

    // Get substitutMoonPlanete normal in tangent space from the normal map
    vec2 coord = vertex_uv * 50.0;
    coord.y = 1.0 - coord.y;
    N = normalize(texture(normal_map, coord).rgb*2.0 - 1.0);
    N = normalize(normal_interp + 0.5*N);

    // Work in tangent space by multiplying our vectors by TBN_mat    
    // Get light direction
    // L = TBN_mat * (light_pos - vertex_position);
    L = (light_pos - vertex_position);
    L = normalize(L);
    
    // Compute diffuse lighting intensity
    float lambertian = max(dot(N, L), 0.0);

    // Get view direction
    //V = TBN_mat * (eye_position - vertex_position);
    // V = TBN_mat * (- vertex_position); // We already applied the view matrix, so the camera is at the origin
    V = (- vertex_position); // We already applied the view matrix, so the camera is at the origin
    V = normalize(V);
    
    // Blinn-Phong specular component
    //H = 0.5*(V + L);
    H = (V + L);
    H = normalize(H);
    
    float spec_angle = max(dot(N, H), 0.0);
    float specular = pow(spec_angle, 128.0);
        
    // Assume all components have the same color but with different weights
    float ambient = 0.4;
    // if (gl_FrontFacing){
        // gl_FragColor = (0.25*ambient + 0.7*lambertian + 1.0*specular)*object_color;
        fragcolor = (0.25*ambient + 0.7*lambertian + 1.0*specular)*object_color;
        // gl_FragColor = texture2D(normal_map, vertex_uv);
    // } else {
    //     gl_FragColor = object_color;
    // }
}
