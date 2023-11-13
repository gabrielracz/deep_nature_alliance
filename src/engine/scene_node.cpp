#include <glm/fwd.hpp>
#include <stdexcept>
#define GLM_FORCE_RADIANS
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <time.h>
#include <glm/gtx/string_cast.hpp>

#include "scene_node.h"

SceneNode::~SceneNode(){
    for(auto child : children) {
        delete child;
    }
}

void SceneNode::Update(double dt){
    elapsed += dt;
    if(!active) {return;}

    // transf_matrix = transform.ScaledMatrix();
    // transf_matrix = transform.ScaledMatrix();
    transform.Update();

    for(auto child : children) {
        child->Update(dt);
    }
}


void SceneNode::SetUniforms(Shader* shader, const glm::mat4& parent_matrix){
    shader->SetUniform4m(transform.GetWorldMatrix(), "world_mat");
    glm::mat4 normal_matrix = glm::transpose(glm::inverse(transform.GetWorldMatrix()));
    shader->SetUniform4m(normal_matrix, "normal_mat");
    shader->SetUniform1f(glfwGetTime(), "timer");
    shader->SetUniform1i(0, "inverted");
}
