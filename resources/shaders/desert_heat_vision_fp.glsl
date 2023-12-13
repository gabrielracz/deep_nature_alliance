#version 330 core

in vec2 uv_interp;

out vec3 FragColor;

uniform sampler2D texture_map;
uniform float timer;

void main() {
    float distanceToCenter = distance(uv_interp, vec2(0.5, 0.5));

    float fade = smoothstep(0.4, 0.6, distanceToCenter);

    // Interpolate between texture color and black based on the fade value
    FragColor = mix(texture(texture_map, uv_interp).rgb, vec3(0.0), fade);
}
