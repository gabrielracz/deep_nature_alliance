#ifndef FP_PLAYER_H_
#define FP_PLAYER_H_

#include "agent.h"
#include "control.h"
#include "defines.h"

class FP_Player : public Agent
{
public:
    FP_Player(const std::string name, const std::string &mesh_id, const std::string shader_id, const std::string &texture_id = "")
        : Agent(name, mesh_id, shader_id, texture_id) {}

    virtual void Update(double dt) override;
    void MouseControls(Mouse& mouse);

    void Init(GLFWwindow *w, Camera *c);

    void PlayerJump();

    void TestMove();

protected:
    void HeadMovement(float dt);

    float bobbing_speed_ = 1.0f;
    float bobbing_amount_ = 0.001f;
    float head_bobbing_ = 0.0f;
    float tilt_angle_ = 0.05f;
    float tilt_smoothing_ = 0.02f;

    float has_dashed_ = false;
    float dash_angle_ = 15.0f;
    float dash_speed_ = 40.0f;
    float sensitivity_ = 0.001f;

private:
    Control control_;

    Camera *camera_;

    void InitEventHandlers(GLFWwindow *w);
};
#endif