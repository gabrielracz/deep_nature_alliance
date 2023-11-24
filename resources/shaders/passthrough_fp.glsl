#version 330 core

in vec2 uv_interp;

out vec3 FragColor;

uniform sampler2D texture_map;

void main() {
    FragColor = vec3(texture(texture_map, uv_interp));
}