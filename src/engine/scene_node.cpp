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
#include "colliders/colliders.h"

SceneNode::SceneNode(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id)
    : name(name), mesh_id(mesh_id), shader_id(shader_id), texture_id(texture_id), collision()
     {}

SceneNode::~SceneNode(){
    for(auto child : children) {
        delete child;
    }
    if(collider != nullptr) {
        delete collider;
    }
}

void SceneNode::Update(double dt){
    elapsed += dt;
    if(!active) {return;}

    // transf_matrix = transform.ScaledMatrix();
    // transf_matrix = transform.ScaledMatrix();
    if(parent) {
        transform.Update(parent->transform.GetWorldMatrix());
    } else {
        transform.Update();
    }

    for(auto child : children) {
        child->Update(dt);
    }
}


void SceneNode::SetUniforms(Shader* shader, const glm::mat4& view_matrix, const glm::mat4& parent_matrix){
    // object transform
    glm::mat4 world = parent_matrix * transform.GetLocalMatrix();
    glm::mat4 normal_matrix = glm::transpose(glm::inverse(view_matrix * world));
    shader->SetUniform4m(world, "world_mat");
    shader->SetUniform4m(normal_matrix,              "normal_mat");

    // material properties
    shader->SetUniform1f(material.texture_repetition,    "texture_repetition");
    shader->SetUniform1f(material.normal_map_repetition, "normal_map_repetition");
    shader->SetUniform1f(material.specular_power,        "specular_power");
    shader->SetUniform1f(material.diffuse_strength,      "diffuse_strength");
    shader->SetUniform1f(material.ambient_additive,      "amb_add");

    // extras
    shader->SetUniform1f(elapsed, "timer");

    // instances
    if(instances.size() > 0) {
        in_camera_instances = shader->SetInstances(instances, view_matrix);
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

void SceneNode::AddChild(SceneNode *n) {
    children.push_back(n);
    n->SetParent(this);
}