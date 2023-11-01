#version 130

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
// in vec2 uv_interp;
in vec3 light_pos;

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform vec4 light_col;


void main() 
{
    vec4 pixel;
    // can start with fixed color...
	pixel = vec4(0.3,0.4,0.6,1.0);
	// ...or color from vertex
	pixel = color_interp;

	// light color (pure white by default)
	// vec4 light_col = vec4(1,1,1,1);

	// view position -- 0 in view space
	vec3 v = vec3(0,0,0);

	// view direction -- object position treated as origin
	vec3 vv = normalize(v - position_interp);
	vec3 lp = vec3(-1,22,30); // static light position, hacked in to demonstrate lighting calculation -- replace with uniform in assignment
	vec3 lv = normalize(lp - position_interp); // light direction, object position as origin
	vec3 h = normalize((vv+lv)/2); // halfway vector -- note /2 pointless, just there for clarity
	vec3 n = normalize(normal_interp); // must normalize interpolated normal
	
	// n.x += 0.2*sin(uv_interp.x*500); // messing with normal to show effect
	n  = normalize(n); // renormalize now that the vector is not a unit vector
	
	float diffuse = dot(n,lv); // N.L -- anything wrong with this line?
	float spec = max(0.0,dot(n,h)); // cannot be negative

	spec = pow(spec,41); // specular power -- instead of hardcoded number, should have uniform here too
	vec4 ambcol = vec4(1,0,0,1); // ambient light -- in this case red, could be anything; should make uniform

//	spec = 0;  // turn off specular
//	diffuse = 0; // turn off diffuse
    float amb = 0.4; // ambient coefficient

	
    // Use variable "pixel", surface color, to help determine fragment color
    gl_FragColor = light_col*pixel*diffuse +
	   light_col*vec4(1,1,1,1)*spec + // specular might not be colored
	   light_col*pixel*amb; // ambcol not used, could be included here
		
   //gl_FragColor = vec4(1,position_interp.x,0,1); // try this -- why does it have the effect it does?
}
