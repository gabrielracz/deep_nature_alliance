#version 400

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;

const float linecount = 120.0;
vec4 gradA = vec4(0.00, 0.0, 0.0, 1.0);
vec4 gradB = vec4(0.8, 0.0, 0.0, 1.0);
vec4 gradC = vec4(1.0, 0.0, 0.0, 1.0);


vec2 pos, uv;

float random(vec2 st){
    return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise(float factor){
    float randomValue = random(uv + timer);
    return factor * randomValue + (1.0 - factor);
}

vec4 base(vec4 pixel)
{
	return pixel * noise(1.0);
}

float triangle(float phase)
{
    float y = sin(phase * 3.14159);
    return pow(y * y, 1.0);
}

float scanline(vec4 pixel, float factor, float contrast)
{
	vec4 v = base(pixel);
	float lum = .2 * v.x + .5 * v.y + .3  * v.z;
	lum *= noise(0.3);
	float tri = triangle(pos.y * linecount);
	tri = pow(tri, contrast * (1.0 - lum) + .5);
	return tri * lum;
}

vec4 gradient(float i)
{
	i = clamp(i, 0.0, 1.0) * 2.0;
	if (i < 1.0) {
		return (1.0 - i) * gradA + i * gradB;
	} else {
		i -= 1.0;
		return (1.0 - i) * gradB + i * gradC;
	}
}

vec4 vignette(vec4 at)
{
	float dx = 1.3 * abs(pos.x - .5);
	float dy = 1.3 * abs(pos.y - .5);
    return at * (1.0 - dx * dx - dy * dy);
}

vec4 gamma(vec4 x, float f)
{
    return pow(x, vec4(1./f));
}

vec4 gamma_red(vec4 x, float f)
{
    return vec4(pow(x.x, 1./f), x.y, x.z, x.a);
}

// Edge detection function
vec4 edgeDetection(vec4 pixel, vec4 edge_color) {
    vec4 offx = texture(texture_map, uv + vec2(0.005, 0));
    vec4 offy = texture(texture_map, uv + vec2(0, 0.005));
    vec4 diff = abs(pixel - offx) + abs(pixel - offy);
    float edginess = diff.r + diff.g + diff.b;
    return mix(vec4(0.0), edge_color, step(0.9, edginess));
}

vec4 edgeDetectionKeep(vec4 pixel, vec4 edge_color) {
    vec4 offx = texture(texture_map, uv + vec2(0.005, 0));
    vec4 offy = texture(texture_map, uv + vec2(0, 0.005));
    vec4 diff = abs(pixel - offx) + abs(pixel - offy);
    float edginess = diff.r + diff.g + diff.b;
    return mix(pixel, edge_color, step(0.9, edginess));
}

// Night vision function
vec4 nightVision(vec4 pixel, vec4 intensities) {
    float luminance = (pixel.r + pixel.g + pixel.b) / 3.0;
    return vec4(luminance * intensities.x, luminance * intensities.y, luminance * intensities.z, 1.0 * intensities.a);
}

vec4 brightness(vec4 pixel, float factor) {
    return clamp(pixel * factor, 0.0, 1.0);
}

vec4 brightness_red(vec4 pixel, float factor) {
    return vec4(clamp(pixel.x * factor, 0.0, 1.0), pixel.y, pixel.z, pixel.a);
}

void main() 
{
    vec4 originalPixel;
    pos = uv = uv0;

    uv.y = floor(uv.y * linecount) / linecount;

    originalPixel = texture(texture_map, uv0);

    

    // Calculate luminance and apply a reddish tint
    gradC = nightVision(originalPixel, vec4(1.0, 0.1, 0.1, 1.0));
    vec4 color_edge = edgeDetection(originalPixel, vec4(1.0, 0.0, 0.0, 1.0));
    vec4 vision = gradient(scanline(originalPixel, 0.6, 2.0));

    // Combine edge color with night vision effect
    //vec4 finalPixel = mix(color_edge, color_night_vision, step(0.1, color_edge.r));

    gl_FragColor = gamma(vision + color_edge, 2.2);
}
