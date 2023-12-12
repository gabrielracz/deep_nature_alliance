#version 330 core

layout (location = 0) in vec3 vertex;

// Function to generate simple noise
float noise(vec2 uv) {
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

uniform float timer;

void main() {
    float speed = 15.0;
    float noiseScale = 0.2;
    float sineFrequency = 2.0;
    float sineAmplitude = 0.2;
    float delay = 0.2;

    float sinnoise = sin(timer * sineFrequency) * sineAmplitude;
    
    float noiseX = noise(vec2(vertex.x * noiseScale, vertex.y * noiseScale + timer * 0.5)) * sinnoise;
    float noiseY = noise(vec2(vertex.y * noiseScale, vertex.z * noiseScale + timer * 0.7)) * sinnoise;
    float noiseZ = noise(vec2(vertex.z * noiseScale, vertex.x * noiseScale + timer * 0.3)) * sinnoise;

    float delayFactor = step(timer, delay);
    
    vec3 noiseOffset = vec3(noiseX, noiseY, noiseZ);
    vec3 newPosition = vertex + normalize(vertex) * speed * (timer - delay * delayFactor) + noiseOffset;
    
    gl_Position = vec4(newPosition, 1.0);
}