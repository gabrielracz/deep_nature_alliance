#ifndef PLAYER_H
#define PLAYER_H

#include "scene_node.h"
#include <glm/matrix.hpp>
class Player : public SceneNode {
public:
    enum class Controls{
        THRUST,
        BRAKE,
        PITCHD,
        PITCHU,
        YAWL,
        YAWR,
        ROLLL,
        ROLLR
    };

    Player(const std::string name, Mesh* mesh, Shader* shader)
    : SceneNode(name, mesh, shader) {}

    virtual void Update(double dt) override;

    void Thrust(int d);
    void ShipControl(Controls c, float damping = 1.0);

    glm::vec3 torque           = {0.0f, 0.0f, 0.0f};
    glm::vec3 ang_acceleration = {0.0f, 0.0f, 0.0f};
    glm::vec3 ang_velocity     = {0.0f, 0.0f, 0.0f};

    glm::vec3 force            = {0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration     = {0.0f, 0.0f, 0.0f};
    glm::vec3 velocity         = {0.0f, 0.0f, 0.0f};

    float mass = 1000;
    float move_speed = 2.5f;
    float accel_amt = 0.25;
    bool braking = false;
    float brake_width = 1.0f;

    

    int lives = 1;
    glm::vec3 angular_velocity {0.0f, 0.0f, 0.0f};
    // glm::vec3 velocity {0.0f, 0.0f, 0.0f};

    glm::mat3 inertia = glm::mat3(
             1337.0f,   0.0f,   0.0f,
               0.0f, 2000.0f,   0.0f,
               0.0f,     0.0,    900.0f);
    glm::mat3 inv_inertia = glm::inverse(inertia);
};

#endif