#version 330 core
out vec4 FragColor;
  
in vec2 uv_interp;

uniform sampler2D depth_map;

void main()
{             
    float depth_value = texture(depth_map, uv_interp).r;
    float near = 0.1;
    float far = 10.0;
    float normalized_depth = depth_value;
    FragColor = vec4(vec3(normalized_depth), 1.0);
}  