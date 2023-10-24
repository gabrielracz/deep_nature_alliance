#ifndef ENEMY_H
#define ENEMY_H

#include "scene_node.h"
class Enemy : public SceneNode {
public:
    Enemy(const std::string name, const Resource *geometry, const Resource *material)
    : SceneNode(name, geometry, material) {}

    virtual void Update(double dt) override;

    glm::vec3 angular_velocity {0.0f, 0.0f, 0.0f};
    float move_speed = 2.75f;
    glm::vec3 velocity {0.0f, 0.0f, 0.0f};
    Transform* target {nullptr};

};

#endif