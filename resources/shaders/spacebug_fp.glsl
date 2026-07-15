#version 400


// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 frag_uv;

uniform sampler2D texture_map;

out vec4 FragColor;

void main (void)
{
    // Very simple fragment shader, but we can do anything we want here
    // We could apply a texture to the particle, illumination, etc.
    vec4 pixel = texture(texture_map, frag_uv);
    FragColor = pixel * frag_color;
}
