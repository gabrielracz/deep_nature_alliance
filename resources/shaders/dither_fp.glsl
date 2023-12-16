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

const int color_palette_size = 16;
// const vec3 color_palette[color_palette_size] = vec3[color_palette_size](
// 	vec3(0.0000, 0.0000, 0.0000),
// 	vec3(0.2863, 0.0000, 0.0000),
// 	vec3(0.0000, 0.2863, 0.0000),
// 	vec3(0.1333, 0.0706, 0.2314),
// 	vec3(1.0000, 0.6275, 0.2667),
// 	vec3(0.7843, 0.2980, 0.0471),
// 	vec3(0.4353, 0.5451, 0.4353),
// 	vec3(0.3608, 0.4392, 1.0000),
// 	vec3(0.9882, 0.8471, 0.6588),
// 	vec3(0.9412, 0.0941, 0.3176),
// 	vec3(0.0000, 0.6588, 0.1882),
// 	vec3(0.3686, 0.8000, 0.9686),
// 	vec3(1.0000, 1.0000, 0.9451),
// 	vec3(0.9686, 0.6118, 0.6118),
// 	vec3(0.7137, 1.0000, 0.2863),
// 	vec3(0.8588, 0.4275, 1.0000)
// );




const vec3 color_palette[16] = vec3[16](
    vec3(0.0, 0.0, 0.0),           // Black
    vec3(0.663, 0.663, 0.663),     // Light Gray
    vec3(0.984, 0.0, 0.0),         // Red
    vec3(0.5, 0.063, 0.063),       // Dark Red
    vec3(0.961, 0.624, 0.031),     // Orange
    vec3(0.5, 0.5, 0.5),           // Gray
    vec3(0.047, 0.882, 0.078),     // Green
    vec3(0.188, 0.5, 0.188),       // Dark Green
    vec3(0.573, 0.926, 0.078),     // Light Green
    vec3(0.0, 0.0, 0.824),         // Blue
    vec3(0.039, 0.0, 0.878),       // Dark Blue
    vec3(0.549, 0.722, 0.922),     // Light Blue
    vec3(0.988, 0.725, 0.894),     // Pink
    vec3(0.945, 0.945, 0.945),     // White
    vec3(0.0, 0.988, 0.0),        // Bright Green
    vec3(0xB6/255.0, 0xFF/255.0, 0x49/255.0)
    // Add the remaining unique NES colors here...
);

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