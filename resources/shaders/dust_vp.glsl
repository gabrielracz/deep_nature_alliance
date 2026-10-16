#version 330 core

// One puff of sand kicked up by a hoof.
// position: random direction in the unit cube, color: three random seeds per particle

layout (location = 0) in vec3 vertex;
layout (location = 2) in vec3 color;

uniform mat4 world_mat;
uniform float timer;           // age of the puff in seconds
uniform float specular_power;  // lifetime of the puff in seconds

out float v_age;
out float v_size;
out float v_visible;

void main() {
    float delay = color.z * 0.12;
    float t = max(timer - delay, 0.0);
    float life = max(specular_power, 0.01);

    // fast initial burst that settles, then a slow rise and drift
    float burst = 1.0 - exp(-t * (2.8 + 2.5 * color.y));
    vec3 p = vec3(vertex.x * 1.7, abs(vertex.y) * 0.9 + 0.25, vertex.z * 1.2) * burst * (4.0 + 5.0 * color.y);
    p.z += burst * (5.0 + 6.0 * color.x);      // thrown back behind the hoof (+z is behind)
    p.y += t * (1.2 + 2.2 * color.x) - t * t * 0.35;
    p.x += sin(t * 1.7 + color.y * 6.28) * t * 0.8;

    gl_Position = world_mat * vec4(p, 1.0);
    v_age = clamp(t / life, 0.0, 1.0);
    v_size = (1.6 + 2.6 * color.x) * (0.6 + 2.4 * burst + t * 1.2);
    v_visible = timer > delay ? 1.0 : 0.0;
}
