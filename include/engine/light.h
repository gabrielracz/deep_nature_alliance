#ifndef LIGHT_H
#define LIGHT_H

#include "transform.h"
#include "shader.h"

class Light {
public:
    Light(const glm::vec4 color) : color(color) {}

    void Update(float dt);

    void SetUniforms(Shader* shader);
    void Attach(Transform* transform);
    Transform transform;
private:
    Transform* parent_transform {nullptr};
    glm::vec3 GetColour() const;
    glm::vec4 color;
    float spec_power = 41;
    float ambient_power = 0.4;

};

#endif