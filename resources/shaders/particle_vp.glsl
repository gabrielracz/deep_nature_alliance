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

void main()
{
    // Let time cycle every four seconds
    float circtime = timer - 4.0 * floor(timer / 4);
    float t = circtime; // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    // Move point along normal and down with t*t (acceleration under gravity)
    position.x += norm.x*t*speed - grav*speed*up_vec.x*t*t;
	
	// add "upward" to y speed to launch the particles vertically -- can easily omit
    position.y += (upward+norm.y)*t*speed - grav*speed*up_vec.y*t*t;

    position.z += norm.z*t*speed - grav*speed*up_vec.z*t*t;
    
    // Now apply view transformation
    gl_Position = view_mat * position;
    // Define outputs
    // Define color of vertex
    //vertex_color = color.rgb; // Color defined during the construction of the particles
    //vertex_color = object_color; // Uniform color 
    vertex_color = vec3(t, 0.0, 1-t); // red-purple dynamic color
    // vertex_color = vec3(1.0, 1-t, 0.0); // red-yellow dynamic color

    // Forward time step to geometry shader
    timestep = t;
}
