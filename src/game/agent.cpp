#define GLM_FORCE_RADIANS
#include "agent.h"

Agent::Agent(const std::string name, const std::string &mesh_id, const std::string shader_id, const std::string &texture_id) : SceneNode(name, mesh_id, shader_id, texture_id)
{

}

void Agent::UpMove()
{
    float step_height = 0.0f;
    if (vertical_velocity_ < 0.0f)
    {
        step_height = step_height_;
    }
    target_position_ = transform.GetPosition() + up_ * (step_height_) + jump_axis_ * ((vertical_offset_ > 0.f ? vertical_offset_ : 0.f));

    // CHECK FOR VERTICAL COLLISION
    // VERTICAL COLLISION RESPONSE
    transform.SetPosition(target_position_);
    step_offset_ = step_height;
}

void Agent::WalkingMove(const glm::vec3 move)
{
    target_position_ = target_position_ + move;

    // COLLISION SWEEPING AND CHECKING HERE FOR FORWARD SIDE TO SIDE OBJECTS

    transform.SetPosition(target_position_);
}

void Agent::DownMove(float dt)
{

    if (vertical_velocity_ > 0.0f)
    {
        return;
    }

    glm::vec3 original_position_ = target_position_;

    float down_velocity = (vertical_velocity_ < 0.0f ? -vertical_velocity_ : 0.0f) * dt;

    if (down_velocity > 0.0 && down_velocity > fall_speed_ && (on_ground_ || !jumping_))
    {
        down_velocity = fall_speed_;
    }

    glm::vec3 drop_amount = up_ * (step_offset_ + down_velocity);
    target_position_ -= drop_amount;

    glm::vec3 position = transform.GetPosition();

    if (target_position_.y <= -30.0)
    { // CHECK FOR BELOW COLLISION

        // Interpolate small fall (would usually want to do this before hitting floor)
        float fraction = (position.y - (-30.0)) * 0.5;
        transform.SetPosition(glm::mix(position, target_position_, fraction));

        // On the ground yo
        vertical_velocity_ = 0.0f;
        vertical_offset_ = 0.0f;
        jumping_ = false;
    }
    else
    { // NO COLLISION i.e FELL FULL HEIGHT
        transform.SetPosition(target_position_);
    }
}

void Agent::Update(double dt)
{
    prev_position_ = transform.GetPosition();

    on_ground_ = OnGround();

    if (glm::length(walk_direction_) > 0)
    {
        walk_direction_ *= glm::pow(1.0f - movement_damping_, dt);
    }

    vertical_velocity_ *= glm::pow(1.0f - movement_damping_, dt);

    vertical_velocity_ -= gravity_ * dt;
    if (vertical_velocity_ > 0.0 && vertical_velocity_ > jump_speed_)
    {
        vertical_velocity_ = jump_speed_;
    }
    if (vertical_velocity_ < 0.0 && glm::abs(vertical_velocity_) > glm::abs(fall_speed_))
    {
        vertical_velocity_ = -glm::abs(fall_speed_);
    }
    vertical_offset_ = vertical_velocity_ * dt;

    UpMove();

    walk_direction_ = strafe_left_ + strafe_right_ + forward_ + backward_;

    if (walk_direction_ != glm::vec3(0.0f))
    {
        walk_direction_ = glm::normalize(walk_direction_) * speed_;
    }

    WalkingMove(walk_direction_);

    DownMove(dt);

    SceneNode::Update(dt);
    //printf("%f %f %f \n", walk_direction_.x, walk_direction_.y, walk_direction_.z);
}

bool Agent::OnGround() const
{
    return (glm::abs(vertical_velocity_) < EPSILON && glm::abs(vertical_offset_) < EPSILON);
}

void Agent::SetLeft(const float amount)
{
    strafe_left_ = glm::vec3(-1.0f, 0.0f, 0.0f) * amount;
}

void Agent::SetRight(const float amount)
{
    strafe_right_ = glm::vec3(1.0f, 0.0f, 0.0f) * amount;
}

void Agent::SetForward(const float amount)
{
    forward_ = glm::vec3(0.0f, 0.0f, -1.0f) * amount;
}

void Agent::SetBackward(const float amount)
{
    backward_ = glm::vec3(0.0f, 0.0f, 1.0f) * amount;
}

void Agent::UnSetLeft()
{
    strafe_left_ = glm::vec3(0.0f);
}

void Agent::UnSetRight()
{
    strafe_right_ = glm::vec3(0.0f);
}

void Agent::UnSetForward()
{
    forward_ = glm::vec3(0.0f);
}

void Agent::UnSetBackward()
{
    backward_ = glm::vec3(0.0f);
}

void Agent::Jump(const glm::vec3 v)
{
    float len = glm::length(v);
    jump_speed_ = len == 0 ? base_jump_speed_ : len;
    vertical_velocity_ = jump_speed_;
    jumping_ = true;

    jump_axis_ = len == 0 ? up_ : glm::normalize(v);
}