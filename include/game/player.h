#ifndef PLAYER_H
#define PLAYER_H

#include "scene_node.h"
class Player : public SceneNode {
public:
    enum class Controls{
        THRUST,
        BRAKE,
        PITCH,
        YAW,
        ROLL
    };

    Player(const std::string name, Mesh* mesh, Shader* shader)
    : SceneNode(name, mesh, shader) {}

    virtual void Update(double dt) override;

    void Thrust(int d);
    void ShipControl(Controls c);

    glm::vec3 torque           = {0.0f, 0.0f, 0.0f};
    glm::vec3 ang_acceleration = {0.0f, 0.0f, 0.0f};
    glm::vec3 ang_velocity     = {0.0f, 0.0f, 0.0f};

    glm::vec3 force            = {0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration     = {0.0f, 0.0f, 0.0f};
    glm::vec3 velocity         = {0.0f, 0.0f, 0.0f};

    float mass = 1000;
    float move_speed = 2.5f;
    float accel_amt = 0.25;

    

    int lives = 1;
    glm::vec3 angular_velocity {0.0f, 0.0f, 0.0f};
    // glm::vec3 velocity {0.0f, 0.0f, 0.0f};

};

#endif