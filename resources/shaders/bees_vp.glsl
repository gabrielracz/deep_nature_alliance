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

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 1.0; // Gravity
float speed = 20.0; // Controls the speed of the explosion
float upward = 3.0; // additional y velocity for all particles

const float pi_over_two = 1.5707963268;
const float pi = 3.1415926536;
const float two_pi = 2.0*pi;

float shash(float value, float minValue, float maxValue) {
    float scaledValue = minValue + (fract(sin(value) * 43758.5453123) + 1.0) * 0.5 * (maxValue - minValue);
    return clamp(scaledValue, minValue, maxValue);
}

vec3 path(float t, float ID) {
    float rad = 100;
    float r = rad;
    float x = (80*cos(2*t)*cos(2*t) + 160*sin(t) + 2*sin(30*t))/2 ;
    float z = (60*sin(t)*cos(0.2*t) + 60*sin((cos(t))) + 100*sin(t))/2;

    return vec3(x, 0.0, z);
}

vec3 movement(float t, float ID) {
    float x = shash(ID, 1.0, 3.0)*cos(ceil(shash(ID, 4.0, 13.0))*t);
    float y = shash(ID, 1.0, 7.0)*sin(ceil(shash(ID, 1.0, 12.0))*t);
    float z = shash(ID, 1.0, 3.0)*cos(ceil(shash(ID, 1.0, 2.0))*t);

    return vec3(x, y, 0.0);

}

vec3 highflyer(float t, float ID) {
    float tt = 10*mod(t, two_pi);
    float y = 201*max(tt*tt*sin(tt), 0.0); // swooping up and gradually back down
    return vec3(0.0, y, 0.0);
}

void main()
{
    float ID = color.r; // red channel stores color
    // Let's first work in model space (apply only world matrix)
    // vec4 position = vec4(vertex, 1.0);
    vec3 position = vertex;
    vec4 norm = normal_mat * vec4(normal, 0.0);

    float phase = shash(ID, 0.0, 2*pi);
    float speed = shash(ID, 0.2, 0.45);
    float param = speed*(timer + phase);
    float t = mod(param, two_pi);
    timestep = t;

    // modulate color to flash and immitate wings
    vec3 highlight = mix(vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.0), shash(ID*23432, 0.0, 1.0)); // brightest color
    vertex_color = vec3(0xE0/255.0, 0x9B/255.0, 0x04/255.0) + (sin(50*t) + 1)/2*highlight; // twinkling yellow

    // general path that the swarm follows
    position += path(t, ID);
    // individual movements
    position += movement(t, ID);

    // control which bees are highflyers cpu-side
    if(color.b > 0.0) {
        position += highflyer(t, ID);
    }
    
    gl_Position = view_mat * world_mat * vec4(position, 1.0);
}
