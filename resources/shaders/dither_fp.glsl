#version 330 core

in vec2 uv_interp;
uniform sampler2D texture_map;

const mat4 dither_kernel = 
    mat4(
        0, 8, 2, 10,
        12, 4, 14, 6,
        3, 11, 1, 9,
        15, 7, 13, 5
    ) * 1.0f/16.0f;

vec4 dither_white(vec4 color) {

    float intensity = (color.r + color.g + color.b)/3.0f;

	//int mult = frag_pos.z < 0.01f ? 1 : 2;
	int mult = 2;
    int pixel_x = int(gl_FragCoord.x)/mult;
    int pixel_y = int(gl_FragCoord.y)/mult;

    float threshhold = dither_kernel[pixel_x % 4][pixel_y % 4] + 0.3f;
    vec4 result;
    if(intensity > threshhold){
        // result = color;
        result = vec4(1.0f);
    } else {
        result = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    return result;
}

const int color_palette_size = 55;
const vec3 color_palette[color_palette_size] = vec3[color_palette_size](
        vec3(0.0000, 0.0000, 0.0000),
        vec3(0.9882, 0.9882, 0.9882),
        vec3(0.9725, 0.9725, 0.9725),
        vec3(0.7373, 0.7373, 0.7373),
        vec3(0.4863, 0.4863, 0.4863),
        vec3(0.6431, 0.8941, 0.9882),
        vec3(0.2353, 0.7373, 0.9882),
        vec3(0.0000, 0.4706, 0.9725),
        vec3(0.0000, 0.0000, 0.9882),
        vec3(0.7216, 0.7216, 0.9725),
        vec3(0.4078, 0.5333, 0.9882),
        vec3(0.0000, 0.3451, 0.9725),
        vec3(0.0000, 0.0000, 0.7373),
        vec3(0.8471, 0.7216, 0.9725),
        vec3(0.5961, 0.4706, 0.9725),
        vec3(0.4078, 0.2667, 0.9882),
        vec3(0.2667, 0.1569, 0.7373),
        vec3(0.9725, 0.7216, 0.9725),
        vec3(0.9725, 0.4706, 0.9725),
        vec3(0.8471, 0.0000, 0.8000),
        vec3(0.5804, 0.0000, 0.5176),
        vec3(0.9725, 0.6431, 0.7529),
        vec3(0.9725, 0.3451, 0.5961),
        vec3(0.8941, 0.0000, 0.3451),
        vec3(0.6588, 0.0000, 0.1255),
        vec3(0.9412, 0.8157, 0.6902),
        vec3(0.9725, 0.4706, 0.3451),
        vec3(0.9725, 0.2196, 0.0000),
        vec3(0.6588, 0.0627, 0.0000),
        vec3(0.9882, 0.8784, 0.6588),
        vec3(0.9882, 0.6275, 0.2667),
        vec3(0.8941, 0.3608, 0.0627),
        vec3(0.5333, 0.0784, 0.0000),
        vec3(0.9725, 0.8471, 0.4706),
        vec3(0.9725, 0.7216, 0.0000),
        vec3(0.6745, 0.4863, 0.0000),
        vec3(0.3137, 0.1882, 0.0000),
        vec3(0.8471, 0.9725, 0.4706),
        vec3(0.7216, 0.9725, 0.0941),
        vec3(0.0000, 0.7216, 0.0000),
        vec3(0.0000, 0.4706, 0.0000),
        vec3(0.7216, 0.9725, 0.7216),
        vec3(0.3451, 0.8471, 0.3294),
        vec3(0.0000, 0.6588, 0.0000),
        vec3(0.0000, 0.4078, 0.0000),
        vec3(0.7216, 0.9725, 0.8471),
        vec3(0.3451, 0.9725, 0.5961),
        vec3(0.0000, 0.6588, 0.2667),
        vec3(0.0000, 0.3451, 0.0000),
        vec3(0.0000, 0.9882, 0.9882),
        vec3(0.0000, 0.9098, 0.8471),
        vec3(0.0000, 0.5333, 0.5333),
        vec3(0.0000, 0.2510, 0.3451),
        vec3(0.9725, 0.8471, 0.9725),
        vec3(0.4706, 0.4706, 0.4706)
);




// const vec3 color_palette[15] = vec3[15](
//     vec3(0.0, 0.0, 0.0),           // Black
//     vec3(0.663, 0.663, 0.663),     // Light Gray
//     vec3(0.984, 0.0, 0.0),         // Red
//     vec3(0.5, 0.063, 0.063),       // Dark Red
//     vec3(0.961, 0.624, 0.031),     // Orange
//     vec3(0.5, 0.5, 0.5),           // Gray
//     vec3(0.047, 0.882, 0.078),     // Green
//     vec3(0.188, 0.5, 0.188),       // Dark Green
//     vec3(0.573, 0.926, 0.078),     // Light Green
//     vec3(0.0, 0.0, 0.824),         // Blue
//     vec3(0.039, 0.0, 0.878),       // Dark Blue
//     vec3(0.549, 0.722, 0.922),     // Light Blue
//     vec3(0.988, 0.725, 0.894),     // Pink
//     vec3(0.945, 0.945, 0.945),     // White
//     vec3(0.0, 0.988, 0.0)        // Bright Green
//     // vec3(0xB6/255.0, 0xFF/255.0, 0x49/255.0)
// );

vec4 dither(vec4 color) {
    float min_distance = 5.0f;
    int closest_color_index = 0;

    for (int i = 0; i < color_palette_size; i++) {
        float distance_to_color = distance(color.rgb, color_palette[i]);
        if (distance_to_color < min_distance) {
            min_distance = distance_to_color;
            closest_color_index = i;
        }
    }
    return vec4(color_palette[closest_color_index], color.a);

    float intensity = (color.r + color.g + color.b) / 3.0;
    int pixel_x = int(gl_FragCoord.x);
    int pixel_y = int(gl_FragCoord.y);
    float threshold = dither_kernel[pixel_x % 4][pixel_y % 4] + 0.3;

    vec4 result;
    if (intensity > threshold) {
        result = vec4(color_palette[closest_color_index], color.a);
    } else {
        result = vec4(0.0);
    }
    return result;
}

void main() {
    vec4 pixel = texture(texture_map, uv_interp);
    pixel = dither(pixel);
    gl_FragColor = pixel;
}