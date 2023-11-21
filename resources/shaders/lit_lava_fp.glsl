#version 330
#pragma optionNV(unroll all)

// Attributes passed from the vertex shader
in vec3 position_interp;
in vec3 normal_interp;
in vec4 color_interp;
in vec2 uv_interp;
in vec3 light_pos;

struct Light {
    vec3 position;
    float ambient_strength;
    vec4 color;
    vec4 ambient_color;
    vec3 direction;
    float spread;
};

in Light lights[3];
flat in int num_lights;

// Uniform (global) buffer
uniform sampler2D texture_map;
uniform float texture_repetition;
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


vec4 lighting(vec4 pixel, int i, vec3 lv, vec3 n) {

	float diffuse = max(0.0, dot(n,lv)); 
    float spec = blinnphong_specular(lv, n);
    // float spec = phong_specular(lv, n);
    if(diffuse == 0.0) {spec = 0.0;}

    return diffuse_strength*diffuse*lights[i].color*pixel + lights[i].ambient_strength*lights[i].ambient_color*pixel + spec*lights[i].color;
}

// taken from:
// https://github.com/patriciogonzalezvivo/lygia/blob/main/generative/snoise.glsl
// MIT license
vec3 mod289(const in vec3 x) { return x - floor(x * (1. / 289.)) * 289.; }
vec4 mod289(const in vec4 x) { return x - floor(x * (1. / 289.)) * 289.; }
vec4 permute(const in vec4 v) { return mod289(((v * 34.0) + 1.0) * v); }
vec4 taylorInvSqrt(in vec4 r) { return 1.79284291400159 - 0.85373472095314 * r; }

float snoise(in vec3 v) {
    const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    vec3 i  = floor(v + dot(v, C.yyy) );
    vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min( g.xyz, l.zxy );
    vec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    vec3 x1 = x0 - i1 + C.xxx;
    vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289(i);
    vec4 p = permute( permute( permute(
                i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
            + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
            + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    float n_ = 0.142857142857; // 1.0/7.0
    vec3  ns = n_ * D.wyz - D.xzx;

    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    vec4 x = x_ *ns.x + ns.yyyy;
    vec4 y = y_ *ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);

    vec4 b0 = vec4( x.xy, y.xy );
    vec4 b1 = vec4( x.zw, y.zw );

    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));

    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    vec3 p0 = vec3(a0.xy,h.x);
    vec3 p1 = vec3(a0.zw,h.y);
    vec3 p2 = vec3(a1.xy,h.z);
    vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                dot(p2,x2), dot(p3,x3) ) );
}

void main() 
{
	vec3 n = normalize(normal_interp); 
    vec4 pixel = texture2D(texture_map, uv_interp * texture_repetition);
    vec2 uv = uv_interp;

    // take a x-y plane slice of the 3D simplex noise that progresses forwards through time
    // -abs() creates ridges instead of rolling hills
    vec3 sample_point = vec3(uv.xy*10.5, timer * 0.5);
    vec3 ridgecol = vec3(0xF2/255.0, 0x38/255.0, 0x00/255.0);
    vec3 backcol = vec3(0x08/255.0, 0x02/255.0, 0x04/255.0);

    float intensity = (-abs(snoise(sample_point))) + 0.6;
    vec3 mixedcol = mix(backcol, ridgecol, intensity);
    pixel = vec4(mixedcol.xyz, 1.0);
    
    // pixel *= color_interp;
    vec4 accumulator = vec4(0.0, 0.0, 0.0, 0.0);
    for(int i = 0; i < num_lights; i++) {
        vec3 lv = normalize(lights[i].position - position_interp); // light direction, object position as origin
        vec4 lit_pixel = lighting(pixel, i, lv, n);
        accumulator += lit_pixel;
    }
    gl_FragColor =  accumulator/num_lights;
}