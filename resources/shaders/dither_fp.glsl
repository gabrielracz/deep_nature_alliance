#version 330 core

in vec2 uv_interp;
uniform sampler2D texture_map;
uniform float timer;

#define M_PI 3.14159265358979323846

//	Classic Perlin 3D Noise 
//	by Stefan Gustavson
//
vec4 permute(vec4 x){return mod(((x*34.0)+1.0)*x, 289.0);}
vec4 taylorInvSqrt(vec4 r){return 1.79284291400159 - 0.85373472095314 * r;}
vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float cnoise(vec3 P){
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = mod(Pi0, 289.0);
  Pi1 = mod(Pi1, 289.0);
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 / 7.0;
  vec4 gy0 = fract(floor(gx0) / 7.0) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 / 7.0;
  vec4 gy1 = fract(floor(gx1) / 7.0) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

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

    // float intensity = (color.r + color.g + color.b) / 3.0;
    // int pixel_x = int(gl_FragCoord.x);
    // int pixel_y = int(gl_FragCoord.y);
    // float threshold = dither_kernel[pixel_x % 4][pixel_y % 4] + 0.3;

    // vec4 result;
    // vec4 boost = vec4(0.2, 0.2, 0.2, 0.0);
    // if (intensity > threshold) {
    //     result = vec4(color_palette[closest_color_index], color.a) + boost;
    // } else {
    //     result = vec4(color_palette[closest_color_index], color.a) - boost;
    // }
    // return result;
}

void main() {
    vec3 sample = vec3(uv_interp.xy*13.0, timer*0.9);
    vec2 uv = uv_interp;
    uv.x += cnoise(sample)*0.005;
    uv.x = clamp(uv.x, 0.0, 1.0);
    
    vec4 pixel = texture(texture_map, uv);

    pixel = dither(pixel);
    gl_FragColor = pixel;
}