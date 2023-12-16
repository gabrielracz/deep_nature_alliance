#ifndef LIGHT_H
#define LIGHT_H

#include "transform.h"
#include "shader.h"

class Light {
public:
    int MAX_LIGHTS = 12;
    Light(const glm::vec4 color) : color(color) {
        SetProjMatrix(glm::ortho(-500.0f, 500.0f, -500.0f, 500.0f, 20.0f, 1300.0f));
    }

    void Update(float dt);

    glm::mat4 CalculateViewMatrix();
    const glm::mat4& GetProjMatrix();

    void SetProjMatrix(const glm::mat4&  proj) { projMatrix = proj;}

    void SetUniforms(ShaderLight& shader_light);
    void Attach(Transform* transform);
    Transform transform;
    glm::vec4 color;
    float spec_power = 41;
    float ambient_power = 0.15;
    
private:


    Transform* parent_transform {nullptr};
    glm::vec3 GetColour() const;

    glm::mat4 projMatrix;
    const float lookAhead = 250.0f;
};

#endif