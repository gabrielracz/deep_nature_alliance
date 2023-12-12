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

    Thrust* thrust1;
    Thrust* thrust2;

   private:
   Game* game;
    //naming would be super fucked this is still ugly but better than forward being up and stuff
    // enum TPControls {
    //     PITCHD = (int)(Player::Controls::W),
    //     PITCHU = (int)(Player::Controls::S),
    //     THRUST = (int)(Player::Controls::SHIFT),
    //     BRAKE = (int)(Player::Controls::CTRL),
    //     YAWL = (int)(Player::Controls::Q),
    //     YAWR = (int)(Player::Controls::E),
    //     ROLLL = (int)(Player::Controls::A),
    //     ROLLR = (int)(Player::Controls::D),
    //     NOTHING = (int)(Player::Controls::SPACE)
    // };
};

#endif