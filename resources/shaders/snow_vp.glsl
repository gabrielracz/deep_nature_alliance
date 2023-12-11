#version 400
// Vertex buffer
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

out vec3 vertex_color;
out float timestep;

// Constants
const float xspeed = 0.05;
const float zspeed = 0.1;
const float yspeed = 0.5;
const float cyclelength = 1.0; // want this pretty long so they can travel to the bottom if high up

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
    // use a bunch of seeds to have random x and z direction and where in the phase we are
    float colorseed = color.x + color.y + color.z;
    float normalseed = normal.x + normal.y + normal.z;
    float phase = hash11(colorseed) * cyclelength;
    float time = mod(timer + phase, cyclelength);
    float idone = (vertex.x + vertex.y + vertex.z) * 100.0;
    vec4 position = world_mat * vec4(vertex, 1.0);
    float s1 = sign(hash11(normalseed) - 0.5);
    float s2 = sign(hash11(colorseed) - 0.5);
    // x and y values this was just a function that worked nicely for snow in the x and y dir
    position.x += (((hash11(idone)*2.0-1.0)*0.5+1.0)*s1*xspeed*time);
    position.z += (((hash11(idone)*2.0+1.0)*0.5-1.0)*s2*zspeed*time);
    position.y -= (yspeed*time);

    // Transform the vertex
    gl_Position = view_mat * position;
    vertex_color = vec3(1.0,1.0,1.0);
    timestep = time;
}
