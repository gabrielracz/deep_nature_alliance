#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

// uniform sampler2D depthMap;

void main()
{
    // float depthValue = texture(depthMap, TexCoords).r;
    FragColor = vec4(0.5, 0.5, 0.5, 1.0);
}