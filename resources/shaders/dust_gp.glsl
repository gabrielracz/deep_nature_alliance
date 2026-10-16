#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 view_mat;
uniform mat4 projection_mat;

in float v_age[];
in float v_size[];
in float v_visible[];

out vec2 g_uv;
out float g_age;
out float g_near;

void main() {
    if (v_visible[0] < 0.5) {
        return;
    }
    // camera facing quad
    vec4 center = view_mat * gl_in[0].gl_Position;
    float s = v_size[0];
    vec2 corners[4] = vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0));
    for (int i = 0; i < 4; i++) {
        g_uv = corners[i];
        g_age = v_age[0];
        g_near = smoothstep(3.0, 16.0, -center.z); // fade puffs right in front of the lens
        gl_Position = projection_mat * (center + vec4(corners[i] * s, 0.0, 0.0));
        EmitVertex();
    }
    EndPrimitive();
}
