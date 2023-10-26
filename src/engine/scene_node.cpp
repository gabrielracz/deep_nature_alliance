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
    if(!active || !visible) {return;}

    shader->Use();
    camera->SetUniforms(shader);
    SetUniforms(shader, parent_matrix);
    // texture->Bind();
    mesh->Draw();

    // remove the scaling factor from our cached transf matrix (children shouldnt scale with parent)
    glm::mat4 t = transf_matrix;
    t[0] = glm::normalize(t[0]);
    t[1] = glm::normalize(t[1]);
    t[2] = glm::normalize(t[2]);

    // glm::mat4 tm = parent_matrix * transform.Matrix(); //temporary hack to avoid sending scale to children, better way coming soon
    glm::mat4 tm = parent_matrix * t; //temporary hack to avoid sending scale to children, better way coming soon
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


void SceneNode::SetUniforms(Shader* shader, const glm::mat4& parent_matrix){
    shader->SetUniform4m(parent_matrix * transf_matrix, "world_mat");

    shader->SetUniform1f(glfwGetTime(), "timer");
    shader->SetUniform1i(0, "inverted");
}
