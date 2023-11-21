#version 330 core

in vec3 texture_coordinates;
uniform samplerCube skybox;

void main() {
        gl_FragColor = texture(skybox, texture_coordinates);
        // gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
}
