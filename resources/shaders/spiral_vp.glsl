#version 400
// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

out vec3 vertex_color;
out float timestep;

// Constants
const float amplitude = 0.5;
const float frequency = 0.5;
const float speed = 5.0;
const float lifetime = 5.0;
const float depth = 40.0;

// hash functions for randomness from here https://tips.orels.sh/optimized-hash-for-shaders
// One in one out
float hash11(float p)
{
    p = fract(p * .1031);
    p *= p + 33.33;
    p *= p + p;
    return fract(p);
}

void main() {
    // Seed the phase based off colour value
    float colorrand = color.x + color.y + color.z;
    float phase = hash11(colorrand) * lifetime;
    // Apply phase so particles dont all do the same thing
    float time = mod(timer + phase, lifetime);

    //Seed random spiral and go for it
    float idone = (vertex.x + vertex.y + vertex.z) * 100.0;
    float phase_offset = fract(idone * 0.1031);

    float y_offset = depth * hash11(idone);
    float mod_amplitude = amplitude * (1.0 - (time + phase) / lifetime);
    //mod_amplitude = max(mod_amplitude, 0.0);
    float dist_to_center = distance(vec2(0.0), vertex.xy);
    // We want a spiral that faces the player so on the x y plane
    float spiral_x = sin(time * speed * frequency + phase_offset) * mod_amplitude;
    float spiral_y = cos(time * speed * frequency + phase_offset) * mod_amplitude;
    vec4 position = world_mat * vec4(vertex.x + spiral_x, vertex.y + spiral_y - y_offset, vertex.z, 1.0);

    gl_Position = view_mat * position;
    vertex_color = vec3(0.5, 0.5, 0.5);
    timestep = time;
}
