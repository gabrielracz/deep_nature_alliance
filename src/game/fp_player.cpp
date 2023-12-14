#define GLM_FORCE_RADIANS
#include "fp_player.h"
#include "colliders/colliders.h"

FP_Player::FP_Player(const std::string name, const std::string &mesh_id, const std::string& shader_id, const std::string &texture_id, Camera *c)
    : Agent(name, mesh_id, shader_id, texture_id), camera_(c) {
        c->Attach(&transform); 
        FPPlayerCollider* col = new FPPlayerCollider(*this, collider_radius_);
        col->SetCallback([this](SceneNode* other) { this->HandleCollisionWith(other); });
        SetCollider(col);
        SetNodeType(TPLAYER);
}

void FP_Player::Control(Controls c, float dt, float damping){
    switch(c) {
        case Player::Controls::W:
            this->SetForward(); 
            break;
        case Player::Controls::S:
            this->SetBackward(); 
            break;
        case Player::Controls::A:
            this->SetLeft(); 
            break;
        case Player::Controls::D:
            this->SetRight();
            break;
        case Player::Controls::Q:
            this->transform.Yaw(turn_speed * dt); 
            break;
        case Player::Controls::E:
            this->transform.Yaw(-turn_speed * dt); 
            break;
        case Player::Controls::SPACE:
            this->PlayerJump();
            break;
        case Player::Controls::UP:
            camera_->transform.Pitch(turn_speed * dt);
            break;
        case Player::Controls::DOWN:
            camera_->transform.Pitch(-turn_speed * dt);
            break;
        default:
            break;  
    }
}

void FP_Player::HeadMovement(float dt)
{
    head_bobbing_ += bobbing_speed_ * dt;

    if (head_bobbing_ > 2 * glm::pi<float>())
    {
        head_bobbing_ = 0.0f;
    }

    float head_offset = (bobbing_amount_ * dt * glm::sin(head_bobbing_)) * 0.5;

    glm::quat bobbing_rotation = glm::angleAxis(head_offset, glm::vec3(1.0f, 0.0f, 0.0f));

    glm::vec3 movement_direction = glm::normalize(transform.GetPosition() - prev_position_);

    // printf("%f\n", glm::length(movement_direction));
    if (glm::length(movement_direction) > 0.1f)
    {
        glm::vec3 move_local;
        if(transform.GetOrientation() != glm::quat()) {
            move_local = glm::inverse(transform.GetOrientation()) * glm::normalize(movement_direction);
        } else {
            move_local = movement_direction;
        }
        glm::vec3 tilt_axis = -glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), move_local);
        glm::quat tilt_quaternion = glm::angleAxis(tilt_angle_, tilt_axis);
        float slerp_factor = (1.0f - glm::pow(1.0f - tilt_smoothing_, dt)) * tilt_speed_;
        glm::quat target_rotation = glm::slerp(camera_->transform.GetOrientation(), tilt_quaternion, slerp_factor);

        camera_->transform.SetOrientation(glm::normalize(target_rotation * bobbing_rotation));
    }
    else
    {
        float slerp_factor = (1.0f - glm::pow(1.0f - tilt_smoothing_, dt)) * tilt_speed_;
        glm::quat target_rotation = glm::slerp(camera_->transform.GetOrientation(), glm::quat(1.0f, 0.0f, 0.0f, 0.0f), slerp_factor);
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
    else if (!has_dashed_ && jumping_ && dash_unlocked_)
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
    if(player_static_) {
        return;
    }
    //printf("X: %f Y: %f Z: %f\n", transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
    Agent::Update(dt);
    HeadMovement(dt);
}

void FP_Player::Reset() {
    transform.SetPosition({0.0f, 0.0f, 0.0f});
}

void FP_Player::MouseControls(Mouse &mouse)
{
    glm::vec2 look = mouse.move * -sensitivity_;
    
    const float max_yaw = 360.0f;
    const float max_pitch = 89.0f;

    transform.Yaw(look.x);
    float yaw = glm::degrees(glm::yaw(camera_->transform.GetOrientation()));
    //float yaw = glm::degrees(2.0f * acos(transform.GetOrientation().w));

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
    //float pitch = glm::degrees(2.0f * acos(camera_->transform.GetOrientation().w));
    float pitch = glm::degrees(glm::pitch(camera_->transform.GetOrientation()));

    if (pitch > max_pitch) {
        float adjust = max_pitch - pitch;
        camera_->transform.Pitch(glm::radians(adjust));
    }
    else if (pitch < -max_pitch + 10.0f) {
        float adjust = -max_pitch + 10.0f - pitch;
        camera_->transform.Pitch(glm::radians(adjust));
    }
}


void FP_Player::HandleCollisionWith(SceneNode* collider)
{
    switch(collider->GetNodeType()) {
        case TLAVA:
            Reset();
            break;
        default:
            break;
    }
}
