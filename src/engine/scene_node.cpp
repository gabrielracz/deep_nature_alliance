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
    delete collider;
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


void SceneNode::SetUniforms(Shader* shader, const glm::mat4& view_matrix){
    // object transform
    glm::mat4 normal_matrix = glm::transpose(glm::inverse(view_matrix * transform.GetWorldMatrix()));
    shader->SetUniform4m(transform.GetWorldMatrix(), "world_mat");
    shader->SetUniform4m(normal_matrix,              "normal_mat");

    // material properties
    shader->SetUniform1f(material.texture_repetition,    "texture_repetition");
    shader->SetUniform1f(material.normal_map_repetition, "normal_map_repetition");
    shader->SetUniform1f(material.specular_power,        "specular_power");
    shader->SetUniform1f(material.diffuse_strength,      "diffuse_strength");

    // extras
    shader->SetUniform1f(glfwGetTime(), "timer");
    shader->SetUniform1i(0, "inverted");

    // instances
    if(instances.size() > 0) {
        shader->SetInstances(instances);
    }
}

void SceneNode::SetNormalMap(const std::string &new_tex_id, float normal_map_repetition) {
    normalmap_id = new_tex_id;
    material.normal_map_repetition = normal_map_repetition;
}

void SceneNode::SetTexture(std::string& new_tex_id, float texture_repetition) {
    texture_id = new_tex_id;
    material.texture_repetition = texture_repetition;
}
