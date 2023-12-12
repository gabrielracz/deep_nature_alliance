#version 330 core

uniform float timer;
out vec4 FragColor;

void main() {
    float transitionDuration = 2.0;
    
    float t = min(timer / transitionDuration, 1.0);
    
    vec3 colorWhite = vec3(1.0); // White
    vec3 colorYellow = vec3(1.0, 0.8, 0.1); // Yellow
    vec3 colorOrange = vec3(1.0, 0.5, 0.0); // Orange
    
    vec3 color = mix(colorWhite, colorYellow, smoothstep(0.0, 0.03, t));
    color = mix(color, colorOrange, smoothstep(0.1, 0.3, t));
    
    FragColor = vec4(color, 1.0);
}
