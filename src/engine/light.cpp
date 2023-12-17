
#include "light.h"
#include <glm/gtc/noise.hpp>
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>

void Light::Update(float dt) {
}

void Light::SetUniforms(ShaderLight& l) {
    glm::vec3 pos = transform.GetPosition();
    if(parent_transform){ 
        l.light_position   = pos + parent_transform->GetPosition();
    } else{
        l.light_position = pos;
    }

    l.color            = color;
    l.ambient_strength = ambient_power;
    l.ambient_color    = color;
}

void Light::Attach(Transform *transform) {
    parent_transform = transform;
}

glm::mat4 Light::CalculateViewMatrix(){
    if (parent_transform){
        return glm::lookAt(parent_transform->GetPosition() + transform.GetPosition(), parent_transform->GetPosition(), glm::vec3(0.0, 1.0, 0.0));
        // glm::quat playerOrientation = parent_transform->GetOrientation();
        // glm::vec3 forwardVector = playerOrientation * glm::vec3(0.0, 0.0, -1.0);
        // return glm::lookAt(parent_transform->GetPosition() + transform.GetPosition(), parent_transform->GetPosition() + lookAhead * forwardVector, glm::vec3(0.0, 1.0, 0.0));
    } else {
        return glm::lookAt(transform.GetPosition(), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    }
}

const glm::mat4& Light::GetProjMatrix(){
    return projMatrix;
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