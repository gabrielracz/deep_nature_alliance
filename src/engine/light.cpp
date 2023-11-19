
#include "light.h"
#include <glm/gtc/noise.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>

void Light::Update(float dt) {
}

void Light::SetUniforms(ShaderLight& l) {
    glm::vec3 pos = transform.GetPosition();
    if(parent_transform){ 
        pos += parent_transform->GetPosition();
    }
    
    l.light_position = pos;
    l.color = color;
    l.ambient_strength = ambient_power;
    l.ambient_color = color;
}

void Light::Attach(Transform *transform) {
    parent_transform = transform;
}

glm::vec3 Light::GetColour(void) const
{
    float flicker_speed = 2.0f;
    float flicker_ambient = 0.1f;
    float flicker_smoothing = 0.5f;
    float flicker_intensity = 0.8f;

    float perlinValue = glm::perlin(glm::vec2(static_cast <float>(glfwGetTime()) * flicker_speed, 0.0));
    float flicker = flicker_intensity * (flicker_ambient + flicker_smoothing * glm::sin(perlinValue));
    return glm::vec3(1 * flicker, 0.8 * flicker, 0.6 * flicker);
}