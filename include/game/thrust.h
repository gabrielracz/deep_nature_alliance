#ifndef THRUST_H
#define THRUST_H
#include "scene_node.h"

class Thrust : public SceneNode  {
    public:
    Thrust(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id = "")
    : SceneNode(name, mesh_id, shader_id, texture_id) {}

    virtual void SetUniforms(Shader* shader, const glm::mat4& view_matrix, const glm::mat4& parent_matrix = glm::mat4(1.0f)) override;
    void ChangeAmount(float a);
    float amount = 0.0f;

};

#endif