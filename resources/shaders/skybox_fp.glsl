#version 330 core

in vec3 texture_coordinates;
uniform samplerCube skybox;

out vec4 FragColor;

void main() {
        FragColor = texture(skybox, texture_coordinates);
}
