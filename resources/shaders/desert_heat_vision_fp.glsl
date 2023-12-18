#version 330 core

in vec2 uv_interp;

out vec3 FragColor;

uniform sampler2D texture_map;

uniform float lastInShade;
uniform float timerJank;

void main() {
    float distanceToCenter = distance(uv_interp, vec2(0.5, 0.5));

    // float fade = smoothstep(0.2, max (0.4, (0.6 - ((timer-lastInShade) * 0.03))), distanceToCenter);
    float fade = smoothstep(0.2, max (0.4, (0.6 - ((timerJank-lastInShade)* 0.01) )), distanceToCenter);

    // Interpolate between texture color and black based on the fade value
    FragColor = mix(texture(texture_map, uv_interp).rgb, vec3(0.0), fade);
}
