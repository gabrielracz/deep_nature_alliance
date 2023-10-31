#ifndef ENEMY_H
#define ENEMY_H

#include "scene_node.h"
class Enemy : public SceneNode {
public:
    Enemy(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "")
    : SceneNode(name, mesh_id, shader_id, texture_id) {}

    virtual void Update(double dt) override;

    glm::vec3 angular_velocity {0.0f, 0.0f, 0.0f};
    float move_speed = 2.75f;
    glm::vec3 velocity {0.0f, 0.0f, 0.0f};
    Transform* target {nullptr};

};

#endif