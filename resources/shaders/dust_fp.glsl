#version 330 core

in vec2 g_uv;
in float g_age;
in float g_near;

uniform float diffuse_strength; // puff intensity

out vec4 FragColor;

void main() {
    float r2 = dot(g_uv, g_uv);
    if (r2 > 1.0) {
        discard;
    }
    float soft = pow(1.0 - r2, 2.5);
    float fade = pow(1.0 - g_age, 1.4) * smoothstep(0.0, 0.1, g_age);
    // lit from above: lighter top, shadowed underside, close to the colour of the sand
    vec3 sand = mix(vec3(0.42, 0.33, 0.24), vec3(0.62, 0.5, 0.36), g_uv.y * 0.5 + 0.5);
    sand = mix(sand, vec3(0.56, 0.47, 0.36), g_age);
    FragColor = vec4(sand, soft * fade * g_near * diffuse_strength * 0.3);
}
