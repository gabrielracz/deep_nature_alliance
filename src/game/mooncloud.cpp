#include "mooncloud.h"

MoonCloud::MoonCloud(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id)
    : SceneNode(name, mesh_id, shader_id, texture_id)
{

}


void MoonCloud::Update(double dt)
{
    current_time_ = fmod((current_time_ + static_cast<float>(dt)), 2.0f * glm::pi<float>());
    
    float new_y = transform.GetPosition().y + (hover_amplitude_ * sin(hover_speed_ * current_time_ + hover_offset_));
    transform.SetPosition({transform.GetPosition().x, new_y, transform.GetPosition().z});

    SceneNode::Update(dt);
}

