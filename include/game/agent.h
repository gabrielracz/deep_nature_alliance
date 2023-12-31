#ifndef AGENT_H_
#define AGENT_H_

#include "player.h"
#include "terrain.h"

class Agent : public Player
{
public:
    Agent(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "");

    virtual void Update(double dt) override;

    virtual void Control(Controls c, float dt, float damping = 1.0) = 0;

    bool OnGround(void) const;

    void SetLeft(const float amount = 1.0f);
    void SetRight(const float amount = 1.0f);
    void SetForward(const float amount = 1.0f);
    void SetBackward(const float amount = 1.0f);
    void UnSetLeft(void);
    void UnSetRight(void);
    void UnSetForward(void);
    void UnSetBackward(void);

    void SetTargetStartPos(const glm::vec3& start) { target_position_ = start; }

    void DownCollision(float collision_point_y = 0.0f);
    void UpCollision();
    void HorizontalCollision();

    void SetTerrain(std::shared_ptr<Terrain> t) { terrain = t; }

    void Jump(glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f));

    float GetHeight() const { return height_; }
    float GetVerticalStep() const  { return vertical_step_height_; }
    //void Reset(void);

    bool on_ground_ = false;
    bool jumping_ = false;

    float vertical_velocity_ = 0.0;
    float vertical_offset_ = 0.0;
    float gravity_ = 9.8 * 3;
    float fall_speed_ = 55.0;
    float jump_speed_ = 30.0;
    float step_height_ = 0.0f;
    float movement_damping_ = 0.0f;
    float step_offset_ = 0.0f;
    float base_jump_speed_ = jump_speed_;
    float speed_ = 0.5f;
    float height_ = 1.0f;
    float vertical_step_height_ = 1.0f;

    // max slope agent can walk up
    float max_walking_angle_ = 35.0f;
    // Slope check for a jump (not totally vertical i.e dash) max angle can traverse
    float max_jumping_angle_ = 45.0f;

protected:
    glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 jump_axis_ = glm::vec3(0.0, 1.0, 0.0f);
    glm::vec3 target_position_ = glm::vec3(0.0f);
    glm::vec3 walk_direction_ = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 forward_ = glm::vec3(0.0f);
    glm::vec3 backward_ = glm::vec3(0.0f);
    glm::vec3 strafe_left_ = glm::vec3(0.0f);
    glm::vec3 strafe_right_ = glm::vec3(0.0f);


    void UpMove(float dt);
    void DownMove(float dt);
    void WalkingMove(const glm::vec3 move, float dt);

private:
    glm::vec3 tempPos;
    const float EPSILON = 1e-6; // For ground checking!
    std::shared_ptr<Terrain> terrain;
};
#endif