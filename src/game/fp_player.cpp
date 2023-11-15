#define GLM_FORCE_RADIANS
#include "fp_player.h"

void FP_Player::Init(GLFWwindow *w, Camera *c)
{
    camera_ = c;
    c->Attach(&transform);
    InitEventHandlers(w);
}

void FP_Player::InitEventHandlers(GLFWwindow *w)
{
    control_.Init(w);

    control_.AddKey(
        GLFW_KEY_W,
        [this](float dt)
        { this->SetForward(); },
        GLFW_REPEAT);

    control_.AddKey(
        GLFW_KEY_S,
        [this](float dt)
        { this->SetBackward(); },
        GLFW_REPEAT);

    control_.AddKey(
        GLFW_KEY_A,
        [this](float dt)
        { this->SetLeft(); },
        GLFW_REPEAT);

    control_.AddKey(
        GLFW_KEY_D,
        [this](float dt)
        { this->SetRight(); },
        GLFW_REPEAT);

    control_.AddKey(
        GLFW_KEY_W,
        [this](float dt)
        { this->UnSetForward(); },
        GLFW_RELEASE);

    control_.AddKey(
        GLFW_KEY_S,
        [this](float dt)
        { this->UnSetBackward(); },
        GLFW_RELEASE);

    control_.AddKey(
        GLFW_KEY_A,
        [this](float dt)
        { this->UnSetLeft(); },
        GLFW_RELEASE);

    control_.AddKey(
        GLFW_KEY_D,
        [this](float dt)
        { this->UnSetRight(); },
        GLFW_RELEASE);

    control_.AddKey(
        GLFW_KEY_SPACE,
        [this](float dt)
        { this->PlayerJump(); },
        GLFW_PRESS);
}

void FP_Player::HeadMovement(float dt)
{
    head_bobbing_ += bobbing_speed_ * dt;

    if (head_bobbing_ > 2 * glm::pi<float>())
    {
        head_bobbing_ = 0.0f;
    }

    float head_offset = (bobbing_amount_ * glm::sin(head_bobbing_)) * 0.5;

    glm::quat bobbing_rotation = glm::angleAxis(head_offset, glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 movement_direction = glm::normalize(transform.GetPosition() - prev_position_);

    // printf("%f\n", glm::length(movement_direction));
    if (glm::length(movement_direction) > 0.1f)
    {
        glm::vec3 tilt_axis = -glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), movement_direction);
        glm::quat tilt_quaternion = glm::angleAxis(tilt_angle_, tilt_axis);
        glm::quat target_rotation = glm::slerp(camera_->transform.GetOrientation(), tilt_quaternion, tilt_smoothing_);

        camera_->transform.SetOrientation(glm::normalize(bobbing_rotation * target_rotation));
    }
    else
    {
        glm::quat target_rotation = glm::slerp(camera_->transform.GetOrientation(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), tilt_smoothing_);

        camera_->transform.SetOrientation(glm::normalize(bobbing_rotation * target_rotation));
    }
}

void FP_Player::PlayerJump()
{
    if (OnGround())
    {
        Jump();
        has_dashed_ = false;
    }
    else if (!has_dashed_)
    {
        glm::vec3 dash_vector = glm::normalize(glm::vec3(0.0f, 0.0f, -10.0f) + glm::vec3(0.0f, glm::abs(sin(dash_angle_)), 0.0f));
        dash_vector = glm::normalize(glm::vec3(transform.GetOrientation() * glm::vec4(glm::normalize(dash_vector), 0.0)));
        Jump(dash_vector * dash_speed_);
        has_dashed_ = true;
    }
}

void FP_Player::TestMove()
{
    strafe_right_ = glm::vec3(1.0f, 0.0f, 0.0f);
}

void FP_Player::Update(double dt)
{
    control_.GetInput(dt);
    Agent::Update(dt);
    HeadMovement(dt);
}

void FP_Player::MouseControls(Mouse &mouse)
{
    glm::vec2 look = mouse.move * -sensitivity_;
    
    const float max_yaw = 360.0f;
    const float max_pitch = 89.0f;

    transform.Yaw(look.x);
    float yaw = glm::degrees(2.0f * acos(transform.GetOrientation().w));

    if (yaw > max_yaw)
    {
        float adjust = max_yaw - yaw;
        transform.Yaw(glm::radians(adjust));
    }
    else if (yaw < -max_yaw)
    {
        float adjust = -max_yaw - yaw;
        transform.Yaw(glm::radians(adjust));
    }

    camera_->transform.Pitch(look.y);
    float pitch = glm::degrees(2.0f * acos(camera_->transform.GetOrientation().w));

    if (pitch > max_pitch) {
        float adjust = max_pitch - pitch;
        camera_->transform.Pitch(glm::radians(adjust));
    }
    else if (pitch < -max_pitch) {
        float adjust = -max_pitch - pitch;
        camera_->transform.Pitch(glm::radians(adjust));
    }
}