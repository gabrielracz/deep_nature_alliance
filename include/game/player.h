#ifndef PLAYER_H
#define PLAYER_H

#include "scene_node.h"
class Player : public SceneNode {
public:
    Player(const std::string name, Mesh* mesh, Shader* shader)
    : SceneNode(name, mesh, shader) {}

    virtual void Update(double dt) override;

    void Thrust(int d);

    float move_speed = 2.5f;
    float accel = 0.25;
    int lives = 1;
    glm::vec3 angular_velocity {0.0f, 0.0f, 0.0f};
    glm::vec3 velocity {0.0f, 0.0f, 0.0f};

};

#endif