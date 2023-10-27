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

SceneNode::SceneNode(const std::string name, Mesh* m, Shader* shd)
: name_(name), mesh(m), shader(shd) {
    // Other attributes
    transform.scale = glm::vec3(1.0, 1.0, 1.0);
}


SceneNode::~SceneNode(){
    for(auto child : children) {
        delete child;
    }
}


const std::string SceneNode::GetName(void) const {

    return name_;
}

void SceneNode::Draw(Camera* camera, const glm::mat4& parent_matrix){
    if(!visible) {return;}

    // might be bad performance

    shader->Use();
    SetUniforms(shader, camera, parent_matrix);
    if(texture != nullptr) {
        texture->Bind();
    }

    if(alpha_enabled) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }

    mesh->Draw();

    glm::mat4 tm = parent_matrix * Transform::RemoveScaling(transf_matrix);  // don't pass scaling to children
    for(auto child : children) {
        child->Draw(camera, tm);
    }
}


void SceneNode::Update(double dt){
    elapsed += dt;
    if(!active) {return;}

    transf_matrix = transform.ScaledMatrix();

    for(auto child : children) {
        child->Update(dt);
    }
}


void SceneNode::SetUniforms(Shader* shader, Camera* camera, const glm::mat4& parent_matrix){
    camera->SetProjectionUniforms(shader, camera_projection);
    shader->SetUniform4m(parent_matrix * transf_matrix, "world_mat");
    shader->SetUniform1f(glfwGetTime(), "timer");
    shader->SetUniform1i(0, "inverted");
}
