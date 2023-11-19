#version 330

// Attributes passed from the vertex shader
in vec3 color_interp;
in vec3 position_interp;
in vec2 uv_interp;
in vec3 normal_interp;
in vec3 light_pos;

uniform sampler2D texture_map;
uniform sampler2D normal_map; // Normal map

uniform float texture_repetition;
uniform float normal_map_repetition;

uniform vec4 light_col;
uniform float specular_power;
uniform float diffuse_strength;
uniform float amb;
uniform vec4 ambcol;
uniform float timer;

float phong_specular(vec3 lv, vec3 n) {
	vec3 v = vec3(0,0,0);
    vec3 r = -lv + 2*(dot(lv, n)*n);
	vec3 vv = normalize(v - position_interp);
    float spec = max(0.0, dot(vv, r));
    spec = pow(spec, specular_power/4); //4x less intense than blinn-phong
    return spec;

}

float blinnphong_specular(vec3 lv, vec3 n) {
	vec3 v = vec3(0,0,0);
	vec3 vv = normalize(v - position_interp);
	vec3 h = normalize((vv+lv)/2); 

    float spec = max(0.0, dot(n,h)); 
    spec = pow(spec, specular_power);

    return spec;
}


vec4 lighting(vec4 pixel, vec3 lv, vec3 n) {

	float diffuse = max(0.0, dot(n,lv)); 
    float spec = blinnphong_specular(lv, n);
    // float spec = phong_specular(lv, n);
    if(diffuse == 0.0) {spec = 0.0;}
		
    return diffuse_strength*diffuse*light_col*pixel + amb*ambcol*pixel + spec*light_col;
}

void main() 
{
	vec3 light_vector = normalize(light_pos - position_interp);                                     // light direction, object position as origin
    vec3 n_bump = normalize(texture(normal_map, uv_interp * normal_map_repetition).rgb*2.0 - 1.0);  // sample normal map
	vec3 normal = normalize(normal_interp + n_bump) ;                                               // displace fragment normal by bump
    vec4 pixel = texture(texture_map, uv_interp * texture_repetition);                              // sample color texture
    // pixel *= color_interp;                                                                       // mix with underlying model color
    vec4 lit_pixel = lighting(pixel, light_vector, normal);
    gl_FragColor = lit_pixel;
}
