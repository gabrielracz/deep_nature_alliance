#version 330 core

in vec2 uv_interp;

out vec3 FragColor;

uniform sampler2D texture_map;

void main() {
    vec4 texColor = texture(texture_map, uv_interp);
    if(texColor.a < 0.1)
        discard;
    FragColor = vec3(texColor);
}