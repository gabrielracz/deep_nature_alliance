#include "beacon.h"
#include "colliders/colliders.h"

Beacon::Beacon(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id)
    : Item(name, mesh_id, shader_id, texture_id)
{
    SphereCollider* col = new SphereCollider(*this, 50.0);
    transform.SetScale({80,80,80});
    col->SetCallback([this](SceneNode* other) { this->HandleCollisionWith(other); });
    SetCollider(col);
    SetNodeType(TBEACON);
}


void Beacon::Update(double dt)
{
    SceneNode::Update(dt);
}

