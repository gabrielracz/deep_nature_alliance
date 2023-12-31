#include "mooneye.h"

MoonEye::MoonEye(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id)
    : SceneNode(name, mesh_id, shader_id, texture_id)
{
    a1_ = glm::linearRand(0.0f, 1.0f);
    a2_ = glm::linearRand(0.0f, 1.0f);
    a3_ = glm::linearRand(0.0f, 1.0f);
    rot_speed_ = glm::linearRand(50.0f, 150.0f);
    radius_ = glm::linearRand(10.0f, 20.0f);
    orbit_speed_ = glm::linearRand(10.0f, 40.0f);
    d1_ = glm::linearRand(-1.0f, 1.0f);
    d2_ = glm::linearRand(-1.0f, 1.0f);
    d3_ = glm::linearRand(-1.0f, 1.0f);
    direction_ = (glm::linearRand(0.0f, 1.0f) < 0.5f) ? 1.0 : -1.0;
}

void MoonEye::Update(double dt)
{   
    current_time_ += static_cast<float>(dt);
    glm::quat orbit_quat = glm::angleAxis(glm::radians(current_time_ * rot_speed_), glm::normalize(glm::vec3(a1_,a2_,a3_)));
    transform.SetOrbit(orbit_quat);

    glm::quat rotation = glm::angleAxis(glm::radians(current_time_ * direction_ * orbit_speed_), glm::normalize(glm::vec3(a1_,a2_,a3_)));
    glm::vec3 orbitPosition = radius_ * glm::rotate(rotation, glm::normalize(glm::vec3(d1_,d2_,d3_)));
    transform.SetPosition(orbitPosition);
    
    SceneNode::Update(dt);
}

