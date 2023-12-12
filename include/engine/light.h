#ifndef LIGHT_H
#define LIGHT_H

#include "transform.h"
#include "shader.h"

class Light {
public:
    int MAX_LIGHTS = 12;
    Light(const glm::vec4 color) : color(color) {}

    void Update(float dt);

    void SetUniforms(ShaderLight& shader_light);
    void Attach(Transform* transform);

    glm::mat4 CalculateLightViewMatrix();

    Transform* GetParentTransform() { return parent_transform; }
    glm::vec3 GetSuperHackyPlayerPlusMeTransformLol() { return parent_transform->GetPosition() + transform.GetPosition(); }

    Transform transform;
    glm::vec4 color;
    float spec_power = 41;
    float ambient_power = 0.15;
private:
    Transform* parent_transform {nullptr};
    glm::vec3 GetColour() const;
    

};

#endif