#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 tex_coord;

// Uniform (global) buffer
uniform sampler2D texture_map;

// Simulation parameters (constants)
uniform vec3 object_color = vec3(0.7, 0.3, 0.03);


void main (void)
{
    // Get pixel from texture
    vec4 outval = texture(texture_map, tex_coord);
    // Adjust specified object color according to the grayscale texture value
    outval = vec4(outval.r*frag_color.r, outval.g*frag_color.g, outval.b*frag_color.b, sqrt(sqrt(outval.r))*frag_color.a);
    // vec4 outval = vec4(1.0, 0.0, 0.0, 1.0);
    // Set output fragment olor
    gl_FragColor = outval;
}
