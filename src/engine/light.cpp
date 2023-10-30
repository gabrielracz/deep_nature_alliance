
#include "light.h"


void Light::SetUniforms(Shader* shader) {
    shader->SetUniform3f(transform.position, "light_pos");
    shader->SetUniform4f(color, "light_col");
}