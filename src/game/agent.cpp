#define GLM_FORCE_RADIANS
#include "agent.h"

Agent::Agent(const std::string name, const std::string &mesh_id, const std::string shader_id, const std::string &texture_id) : SceneNode(name, mesh_id, shader_id, texture_id)
{
}

void Agent::UpMove(float dt)
{
    float step_height = 0.0f;
    if (vertical_velocity_ < 0.0f)
    {
        step_height = step_height_;
        return;
    }
    glm::vec3 up_position_ = transform.GetPosition() + up_ * (step_height_) + jump_axis_ * ((vertical_offset_ > 0.f ? vertical_offset_ : 0.f));
    // CHECK FOR VERTICAL COLLISION
    // VERTICAL COLLISION RESPONSE

    // DASH NO CLIP FIX GANG
    if (terrain->SamplePassable(up_position_.x, up_position_.z)) {
            return;
    }

    //Check if we not doing a raw up jump
    if (jump_axis_.x != 0.0f || jump_axis_.z != 0.0f) {
        glm::vec3 sample_normal = terrain->SampleNormal(up_position_.x, up_position_.z);
        float sample_height = terrain->SampleHeight(up_position_.x, up_position_.z);
        float slope_angle = glm::degrees(glm::acos(glm::dot(sample_normal, up_)));
    
        if (slope_angle > max_jumping_angle_ && up_position_.y < sample_height) {
            vertical_velocity_ = 0.0f;
            vertical_offset_ = 0.0f;
            jumping_ = false;
            return;
        }
    }

    //we not being sus lets go!
    target_position_ = up_position_;
    transform.SetPosition(target_position_);
    step_offset_ = step_height;
}

void Agent::WalkingMove(const glm::vec3 move, float dt)
{

    if (glm::length(move) > 0.001)
    {
        glm::vec3 forward = glm::normalize(glm::vec3(transform.GetOrientation() * glm::vec4(glm::normalize(move), 0.0)));
        glm::vec3 target_step_ = target_position_ + forward * speed_;
        if (terrain->SamplePassable(target_step_.x, target_step_.z)) {
            return;
        }
        float terrain_nextY = terrain->SampleHeight(target_step_.x, target_step_.z);
        glm::vec3 sample_normal = terrain->SampleNormal(target_step_.x, target_step_.z);
        float slope_angle = glm::degrees(glm::acos(glm::dot(sample_normal, forward))) - 90;
        //printf("Slope forward angle %f \n", slope_angle);
        if (slope_angle < max_walking_angle_ || target_position_.y > terrain_nextY + height_ * 4) { 
            target_position_ += forward * speed_ * dt * 100.0f;
            transform.SetPosition(target_position_);
        }
    }

    // COLLISION SWEEPING AND CHECKING HERE FOR FORWARD SIDE TO SIDE OBJECTS
}

void Agent::DownMove(float dt)
{
    glm::vec3 original_position_ = target_position_;

    float down_velocity = (vertical_velocity_ < 0.0f ? -vertical_velocity_ : 0.0f) * dt;

    if (down_velocity > 0.0 && down_velocity > fall_speed_ && (on_ground_ || !jumping_))
    {
        down_velocity = fall_speed_;
    }

    glm::vec3 drop_amount = up_ * (step_offset_ + down_velocity);
    target_position_ -= drop_amount;

    glm::vec3 position = transform.GetPosition();

    float terrainY = terrain->SampleHeight(target_position_.x, target_position_.z);
    
    if (target_position_.y < terrainY + height_) {
        //We are touching terrain!

        glm::vec3 sample_normal = terrain->SampleNormal(target_position_.x, target_position_.z);
        glm::vec3 ground_parallel = glm::cross(up_, sample_normal);
        glm::vec3 down_slope = glm::cross(ground_parallel, sample_normal);
        float slope_angle = glm::degrees(glm::acos(glm::dot(sample_normal, up_)));

        if (slope_angle >= max_walking_angle_) {
            target_position_ += glm::normalize(down_slope) * 0.5f;
            jumping_ = false;
        } else {
            vertical_velocity_ = 0.0f;
            vertical_offset_ = 0.0f;
            jumping_ = false;
        }
        target_position_.y = terrainY + height_;
        transform.SetPosition(target_position_);
    } else {
        // We are falling
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

    UpMove(dt);

    walk_direction_ = strafe_left_ + strafe_right_ + forward_ + backward_;

    if (walk_direction_ != glm::vec3(0.0f))
    {
        walk_direction_ = glm::normalize(walk_direction_);
    }

    WalkingMove(walk_direction_, dt);

    DownMove(dt);

    SceneNode::Update(dt);
    //printf("%f %f %f \n", walk_direction_.x, walk_direction_.y, walk_direction_.z);
    //printf("%f %f %f \n", transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);
}

//Collision for after the fact can be used inplace of current terrain check in DownStep
void Agent::DownCollision(float collision_point_y)
{
    if (target_position_.y <= (collision_point_y + height_))
    {
        glm::vec3 position = transform.GetPosition();

        float fraction = (position.y - (collision_point_y + height_)) * 0.5;
        //prob want to set prev_position_.y to collision_point_y here if doesnt work as intended
        transform.SetPosition(glm::vec3(position.x, glm::mix(prev_position_.y, position.y, fraction), position.z));
        // On the ground yo
        vertical_velocity_ = 0.0f;
        vertical_offset_ = 0.0f;
        jumping_ = false;
    }
}

void Agent::UpCollision()
{
    vertical_velocity_ = 0.0f;
    vertical_offset_ = 0.0f;
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