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
uniform float thrust_amount;

// Attributes forwarded to the geometry shader
out vec4 particle_color;
out float particle_id;

// Simulation parameters (constants)
uniform vec3 out_vec = vec3(0.0, 0.0, 1.0); // Up direction
float accel = 4.2; // An acceleration applied to the particles coming from some attraction force
float speed = 10.98; // Control the speed of the motion

// Define some useful constants
const float pi = 3.1415926536;
const float pi_over_two = 1.5707963268;
const float two_pi = 2.0*pi;

float shash(float value, float minValue, float maxValue) {
    float scaledValue = minValue + (fract(sin(value) * 43758.5453123) + 1.0) * 0.5 * (maxValue - minValue);
    return clamp(scaledValue, minValue, maxValue);
}

void main()
{
    // Define particle id
    particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles

    // Define time in a cyclic manner
    // float phase = two_pi*particle_id; // Start the sin wave later depending on the particle_id
    float phase = shash(particle_id, 0.0, two_pi); // Start the sin wave later depending on the particle_id
    float param = timer + phase; // The constant that divides "timer" also helps to adjust the "speed" of the fire
    float rem = mod(12*param*vertex.z, pi_over_two); // Use the remainder of dividing by pi/2 so that we are always in the range [0..pi/2] where sin() gives values in [0..1]
    float circtime = sin(rem); // Get time value in [0..1], according to a sinusoidal wave
                                    
    // Set up parameters of the particle motion
    float t = abs(circtime/2.8)*(0.3 + abs(normal.z)); 

    float spd = speed * thrust_amount;

    vec3 position = vertex;
    position += spd*out_vec*accel*t*t; // Particle moves up
    
    gl_Position = view_mat * world_mat * vec4(position, 1.0);
    
    // Define amount of blending depending on the cyclic time
    vec4 blue_heat = 1/(position.z + 2) * vec4(0.0, 0.7, 3.0, 1.0);
    vec4 fire_col = vec4(0.6, 0.225, 0.01, 1.0)*1.5;
    particle_color = fire_col;// + blue_heat;
    float alpha = pow(thrust_amount, 3) * (1.0 - (circtime*circtime)/2);
    particle_color.a = alpha;
}