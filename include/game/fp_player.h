#ifndef FP_PLAYER_H_
#define FP_PLAYER_H_

#include "agent.h"
#include "control.h"
#include "defines.h"

class FP_Player : public Agent
{
public:
    FP_Player(const std::string name, const std::string &mesh_id, const std::string& shader_id, const std::string &texture_id, Camera *c);

    virtual void Update(double dt) override;
    virtual void Control(Controls c, float dt, float damping = 1.0) override;
    virtual void MouseControls(Mouse& mouse) override;

    virtual void HandleCollisionWith(SceneNode* collider) override;

    void PlayerJump();
    void Reset();
    void TestMove();

    void UnlockDash() {dash_unlocked_ = true;}

protected:
    void HeadMovement(float dt);

    float bobbing_speed_ = 1.0f;
    float bobbing_amount_ = 0.02f;
    float head_bobbing_ = 0.0f;
    float tilt_angle_ = 0.15f;
    float tilt_smoothing_ = 0.02f;
    float tilt_speed_ = 10.0f;
    float turn_speed =  1.5f;

    float has_dashed_ = false;
    float dash_angle_ = 15.0f;
    float dash_speed_ = 40.0f;
    float sensitivity_ = 0.001f;

    float collider_radius_ = 2.0f;
    bool dash_unlocked_ = false;

private:
//  enum FPControls {
//      FORWARD = (int)(Player::Controls::W),
//      BACK = (int)(Player::Controls::S),
//      SPRINT = (int)(Player::Controls::SHIFT),
//      CROUCH = (int)(Player::Controls::CTRL),
//      SPINL = (int)(Player::Controls::Q),
//      SPINR = (int)(Player::Controls::E),
//      LEFT = (int)(Player::Controls::A),
//      RIGHT = (int)(Player::Controls::D),
//      JUMP = (int)(Player::Controls::SPACE)
//  };

 Camera* camera_;

 // void InitEventHandlers(GLFWwindow *w);
};
#endif