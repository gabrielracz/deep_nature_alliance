// Material with no illumination simulation

#version 330

// Attributes passed from the vertex shader
in vec4 color_interp;
uniform int inverted;
uniform float timer;


void main() 
{
    vec4 col = vec4(abs((timer*6.0)*inverted - color_interp.x),
                    abs(cos(timer*6.0)*inverted - color_interp.y),
                    abs(sin(timer*6.0)*inverted - color_interp.z),
                    1.0f);
    // vec4 col = color_interp;
	gl_FragColor = col;
	//gl_FragColor = vec4(0.6, 0.6, 0.6, 1.0);
}
