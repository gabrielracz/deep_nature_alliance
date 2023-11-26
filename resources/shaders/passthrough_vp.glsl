#version 330 core

layout(location=0) in vec3 vertex;
layout(location=1) in vec2 uv;

out vec2 uv_interp;

void main() {
    gl_Position = vec4(vertex.xy, 0.0, 1.0);
    uv_interp = uv;
}