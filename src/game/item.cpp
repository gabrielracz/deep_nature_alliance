#include "item.h"
#include "colliders/colliders.h"

Item::Item(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id)
    : SceneNode(name, mesh_id, shader_id, texture_id)
{
<<<<<<< HEAD
    SphereCollider* col = new SphereCollider(*this, item_col_radius_, true);
    col->SetCallback([this](SceneNode* other) { this->HandleCollisionWith(other); });
=======
    SphereCollider* col = new SphereCollider(*this, item_col_radius_);
    col->SetCallback([this](SceneNode& other) { this->HandleCollisionWith(&other); });
>>>>>>> memory_performance_prob_not_worth
    SetCollider(col);
    SetNodeType(TITEM);
}

void Item::Update(double dt){

    current_time_ = fmod((current_time_ + static_cast<float>(dt)), 2.0f * glm::pi<float>());
    
    float new_y = transform.GetPosition().y + (hover_amplitude_ * sin(hover_speed_ * current_time_ + hover_offset_));
    transform.SetPosition({transform.GetPosition().x, new_y, transform.GetPosition().z});

    SceneNode::Update(dt);
}

void Item::HandleCollisionWith(SceneNode* collider) {
    if (callback) {
        callback();
        if (del_on_collect_) {
            deleted = true;
        }
    }
}