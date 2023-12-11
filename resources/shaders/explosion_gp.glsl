#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 world_mat;

void main() {
    vec4 pos = gl_in[0].gl_Position;

    for (int i = 0; i < 36; ++i) {
        float angle = radians(float(i * 10));
        float x = cos(angle);
        float y = sin(angle);

        gl_Position = projection_mat * view_mat * world_mat * (pos + vec4(x, y, 0.0, 0.0));
        EmitVertex();
    }
    EndPrimitive();
}