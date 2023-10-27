// Source code of fragment shader
#version 330

// Attributes passed from the vertex shader
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;
uniform vec4 text_color;
uniform vec4 background_color;

// Configuration of the font texture
// Number of characters per row
const int num_columns = 16;
// Number of rows in the texture
const int num_rows = 6;
// Size of a character in texture units
float char_width = 1.0/float(num_columns);
float char_height = 1.0/float(num_rows);

// Text input
uniform int text_len;
uniform int text_content[512];

void main()
{
        //gl_FragColor = texture2D(onetex, uv_interp);
		//return;
//	gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
//	return;
    // Check if we are writing on this sprite
    if (text_len > 0){
        // Find which character we are writing in the block covered by
        // this fragment
        // Get the index of the character in the text string according
        // to the x texture coordinate
        int string_index = int(floor(uv_interp.x*float(text_len)));

        // Get the character code
        int char_index = text_content[string_index] - 32; // Map space character to 0

        // Get character's row and column in the font texture
        //int row = char_index / num_columns;
        //int col = char_index - row*num_columns;
        int row = char_index / num_columns;
        int col = char_index % num_columns;

        // Map the fragment texture coordinates to a value between 0 and
        // 1 for a character
		
		float cuv_width = 1.0f/text_len;
		float cuv_diff = uv_interp.x - cuv_width*string_index;
		float cuv_x = cuv_diff / cuv_width;
		float cuv_y = uv_interp.y;

		//vec2 cuv;
		//cuv.x = uv_interp.x*float(text_len) - float(string_index);
		//cuv.y = uv_interp.y;
        // Map the value between 0 and 1 to a coordinate in the font
        // texture
        vec2 fuv;
        fuv.x = (cuv_x + col)*char_width;
        fuv.y = clamp(1.0f - ((1.0f - cuv_y) + row)*char_height, 0, 1.0f);

        //fuv.y = 1.0f-(row*char_height) - (1 - cuv_y/num_rows);
		//gl_FragColor = vec4(row*10/255.0f, col*10/255.0f, 0.0f, 1.0f);
		//return;
		//return;

        // Draw character
		vec4 texture_color = texture2D(onetex, fuv);
		vec4 color = texture_color * text_color;
        if(color.a < 1.0f) {
            gl_FragColor = background_color;
        } else {
            gl_FragColor = color;
        }
    } 
}
