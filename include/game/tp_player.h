#ifndef TP_PLAYER_H
#define TP_PLAYER_H

#include <glm/matrix.hpp>
#include "collision_manager.h"
#include "player.h"
#include "thrust.h"

class Game;

class Tp_Player : public Player {
   public:
    Tp_Player(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id = "", Game* game = nullptr);

    virtual void Update(double dt) override;

    virtual void Control(Controls c, float dt, float damping = 1.0) override;
    virtual void MouseControls(Mouse& mouse) override;

    virtual void SetUniforms(Shader* shader, const glm::mat4& view_matrix, const glm::mat4& parent_matrix = glm::mat4(1.0f)) override;

    // void Thrust(int d);
    // void ShipControl(Controls c, float damping = 1.0);

    glm::vec3 torque = {0.0f, 0.0f, 0.0f};
    glm::vec3 ang_acceleration = {0.0f, 0.0f, 0.0f};

    glm::vec3 force = {0.0f, 0.0f, 0.0f};
    glm::vec3 acceleration = {0.0f, 0.0f, 0.0f};

    float mass = 1000;
    float move_speed = 2.5f;
    float accel_amt = 0.25;
    bool braking = false;
    float brake_width = 1.0f;
    bool thrusting = false;
    float collider_radius_ = 5.0f;

    int lives = 1;
    glm::vec3 angular_velocity{0.0f, 0.0f, 0.0f};
    // glm::vec3 velocity {0.0f, 0.0f, 0.0f};

    glm::mat3 inertia = glm::mat3(
        1000.0f,
        0.0f,
        0.0f,
        0.0f,
        1700.0f,
        0.0f,
        0.0f,
        0.0,
        1000.0f);
    glm::mat3 inv_inertia = glm::inverse(inertia);

    Thrust* thrust1 = nullptr;
    Thrust* thrust2 = nullptr;
    Thrust* thrust3 = nullptr;

   private:
   Game* game;
   void ConfigureThrusters(double dt);
};

#endif