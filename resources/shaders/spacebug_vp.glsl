#version 400
// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

out vec3 vertex_color;
out float timestep;
out vec2 uv_interp;

// Constants
const float minSpeed = 0.05; 
const float maxSpeed = 0.1; 
const float spiralDuration = 0.1;
const float spiralFactor = 0.5; 
const float spiralSpeedFactor = 1.0;
const float cycleTime = 40.0;
const float clumpFactor = 5.0;

const int numPositions = 10;
vec3 positions[numPositions];

// Random functions and stuff were inspired by stuff found on shadertoy
// This function was ripped from this shader tho https://www.shadertoy.com/view/Xt23Ry
float rand(float co) { return fract(sin(co*(91.3458)) * 47453.5453); }

// Note: all these random values just come from looking at coutless shadertoy examples
//       and seeing that random primes of hundreds just work for this random generation application
// Generate random positions to travel between but have a little fun
void generateRandomPositionsWeird(float id, vec3 initialPos) {

    for (int i = 0; i < numPositions; ++i) {
        float seed = id + float(i); 
        float randX = (rand(seed * 356.789) - 0.5) * rand(id) * clumpFactor;
        float randY = (rand(seed * 789.123) - 0.5) * mod(id, clumpFactor);
        float randZ = (rand(seed * 135.246) - 0.5) * 1.0 * clumpFactor;

        positions[i] = initialPos + vec3(randX , randY, randZ);
    }
}

// Generate random positions to travel between
void generateRandomPositions(float id, vec3 initialPos) {
    for (int i = 0; i < numPositions; ++i) {
        float seed = id + float(i) * 10.0; 
        float randX = (rand(seed * 356.789) - 0.5) * clumpFactor;
        float randY = (rand(seed * 789.123) - 0.5) * clumpFactor;
        float randZ = (rand(seed * 135.246) - 0.5) * clumpFactor;

        positions[i] = initialPos + vec3(randX, randY, randZ);
    }
}
// How fast does this particle go?
float calculateSpeed(float id) {
    float baseSpeed = rand(id);
    float speedRange = maxSpeed - minSpeed;
    return baseSpeed * speedRange + minSpeed;
}

float calculateParticleID(vec3 position) { // Get your random seed
    return (position.x + position.y + position.z) * 100.0;
}

void main() {
    float id = calculateParticleID(vertex);
    generateRandomPositionsWeird(id, vertex);
    float particleSpeed = calculateSpeed(id);
    // Phase caclulation so phase of particle can be different
    float time = mod(timer + rand(vertex.x + vertex.y + vertex.z * 100.0) * 100.0, cycleTime);
    // Curent traveling to point is decided by our time we need to mod and floor this so its a nice index
    int currentIndex = int(floor(mod(time * particleSpeed, float(numPositions))));
    int nextIndex = int(mod(float(currentIndex + 1), float(numPositions)));
    vec3 initialPosition = positions[currentIndex];
    vec3 targetPosition = positions[nextIndex];
    float interpFactor = fract(time * particleSpeed);
    // Interpolate between target position and last position
    vec3 newPosition = mix(initialPosition, targetPosition, interpFactor);

    
    // Add spiral to travel interpolation
    float spiralTime = time * particleSpeed * spiralSpeedFactor;
    //6.28 just approxes 2 pi
    // apply spiral parametric
    vec3 spiralOffset = vec3(cos(spiralTime * 6.28), sin(spiralTime * 6.28), sin(spiralTime * 0.5)) * spiralFactor;
    //alter position
    newPosition += spiralOffset;
    initialPosition = newPosition;

    // Transform the vertex
    gl_Position = view_mat * world_mat * vec4(newPosition, 1.0);
    //float grey = min(abs(sin(time*(id))), 0.5);
    vertex_color = vec3(0.5, 0.5 - mod(time, 1.0), 0.3 + (mod(sin(time),1.0) - 0.5));
    //vertex_color = vec3(grey,grey,grey);
    //vertex_color = vec3(1.0, 1.0, 1.0);
    timestep = time;
    uv_interp = uv;
}
