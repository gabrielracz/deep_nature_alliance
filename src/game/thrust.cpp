#include "thrust.h"

void Thrust::SetUniforms(Shader* shader, const glm::mat4& view_matrix, const glm::mat4& parent_matrix) {
    shader->SetUniform1f(amount, "thrust_amount");
    SceneNode::SetUniforms(shader, view_matrix, parent_matrix);
}

void Thrust::ChangeAmount(float a) {
    amount = glm::clamp(amount + a, 0.0f, 1.0f);
}