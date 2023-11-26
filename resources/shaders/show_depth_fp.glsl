#version 330 core
out vec4 FragColor;
  
in vec2 uv_interp;

uniform sampler2D depth_map;

void main()
{             
    float depth_value = texture(depth_map, uv_interp).r;
    float zNear = 0.1;
    float zFar = 400.0;
    float normalized_depth = (2*zNear ) / (zFar + zNear - depth_value*(zFar -zNear));
    FragColor = vec4(vec3(normalized_depth), 1.0);
    FragColor = vec4(vec3(depth_value), 1.0);
}  