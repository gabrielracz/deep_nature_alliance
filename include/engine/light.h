#ifndef LIGHT_H
#define LIGHT_H

#include "transform.h"
#include "shader.h"

class Light {
public:
    Light(const glm::vec4 color) : color(color) {}

    void SetUniforms(Shader* shader);
    Transform transform;
private:
    glm::vec4 color;
};

#endif