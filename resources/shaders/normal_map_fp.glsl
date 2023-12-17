#version 330
#pragma optionNV(unroll all)

// Attributes passed from the vertex shader
in vec3 color_interp;
in vec3 position_interp;
in vec2 uv_interp;
in vec3 normal_interp;
in vec3 light_pos;

in vec4 shadow_space_pos;

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

uniform float texture_repetition;
uniform float normal_map_repetition;

// uniform vec4 light_col;
uniform float specular_power;
uniform float specular_coefficient;
uniform float diffuse_strength;
uniform float amb;
uniform float amb_add;
uniform vec4 ambcol;
uniform float timer;

uniform sampler2D texture_map;
uniform sampler2D normal_map; // Normal map
uniform sampler2D shadow_map;

layout(location=0) out vec3 FragColor;

bool in_shadow;

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


vec4 lighting(vec4 pixel, int i, vec3 lv, vec3 n, float shadow) {

	float diffuse = max(0.0, dot(n,lv));
    float spec = blinnphong_specular(lv, n);
    // float spec = phong_specular(lv, n);
    if(diffuse == 0.0 || specular_power == 0.0) {spec = 0.0;}

    vec4 lit = (lights[i].ambient_strength+amb_add)*lights[i].ambient_color*pixel + (1.1 - shadow)*(
               diffuse_strength*diffuse*lights[i].color*pixel + 
               specular_coefficient*spec*lights[i].color);

    return lit;
}

float PCSSShadowCalculation(vec4 fragPosLightSpace) {
   vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
   projCoords = projCoords * 0.5 + 0.5;

   float currentDepth = projCoords.z;
   float bias = 0.0004;

   // PCSS
   vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
   vec2 blockerSearchSize = vec2(1.0); // Initial blocker search size
   float penumbraWidth = 0.0; // Initial penumbra width

   // Search for the blocker
   for(int x = -3; x <= 3; ++x){
       for(int y = -3; y <= 3; ++y){
           float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
           if(currentDepth - bias > pcfDepth){
               blockerSearchSize = vec2(x, y) * texelSize;
               break;
           }
       }
   }

   // Calculate penumbra width
   for(int x = -3; x <= 3; ++x){
       for(int y = -3; y <= 3; ++y){
           float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
           if(currentDepth - bias > pcfDepth){
               penumbraWidth = max(penumbraWidth, length(vec2(x, y) * texelSize - blockerSearchSize));
           }
       }
   }

   // Calculate shadow
   float shadow = 0.0;
   for(int x = -3; x <= 3; ++x){
       for(int y = -3; y <= 3; ++y){
           float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
           float distanceFromBlocker = length(vec2(x, y) * texelSize - blockerSearchSize);
           float falloff = smoothstep(0.0, penumbraWidth, distanceFromBlocker);
           shadow += currentDepth - bias > pcfDepth ? falloff : 0.0;       
       }   
   }

   shadow /= 49.0;

   shadow *= 1.25; //darkness modifier

   // Outside shadow map
   if(projCoords.x > 1.0 || projCoords.x < 0.0 || projCoords.y > 1.0 || projCoords.y < 0.0) {
       shadow = 0.0;
   }
   return shadow;
}

float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)

    // simple PCF, average surrounding 9 texels
    float currentDepth = projCoords.z;
    float bias = 0.0004;
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadow_map, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadow_map, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // get depth of current fragment from light's perspective
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // outside shadow map
    if(projCoords.x >= 1.0 || projCoords.x < 0.0 || projCoords.y >= 1.0 || projCoords.y < 0.0) {
            shadow = 0.0;
    }
    return shadow;
}  



void main() {
    vec4 accumulator = vec4(0.0, 0.0, 0.0, 1.0);
    for(int i = 0; i < num_lights; i++) {
        vec3 light_vector = normalize(lights[i].position - position_interp);                                     // light direction, object position as origin
        vec3 n_bump = normalize(texture(normal_map, uv_interp * normal_map_repetition).rgb*2.0 - 1.0);  // sample normal map
        vec3 normal = normalize(normal_interp + n_bump) ;                                               // displace fragment normal by bump
        vec4 pixel = texture(texture_map, uv_interp * texture_repetition);                              // sample color texture
        if(pixel.a < 0.1)
            discard;
        // vec4 pixel = vec4(color_interp, 1.0);                                                                       // mix with underlying model color
        float shadow = PCSSShadowCalculation(shadow_space_pos);
        // if(shadow > 0.0) {
        //     in_shadow = true;
        //     // accumulator = vec4(1.0, 0.0, 1.0, 1.0);
        // }
        vec4 lit_pixel = lighting(pixel, i, light_vector, normal, shadow);
        // lit_pixel.a = 1.0f;
        accumulator += lit_pixel;
    }
    gl_FragColor = accumulator ;
    // FragColor = vec3(accumulator);
    // float depth = gl_FragCoord.w * 4 + 0.1;
    // FragColor = vec3(depth, depth, depth);
}
