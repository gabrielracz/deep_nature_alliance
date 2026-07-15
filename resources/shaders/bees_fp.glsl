#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;

out vec4 FragColor;

void main (void)
{
    // Very simple fragment shader, but we can do anything we want here
    // We could apply a texture to the particle, illumination, etc.

    FragColor = frag_color;
}
