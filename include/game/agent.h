#ifndef AGENT_H_
#define AGENT_H_

#include "scene_node.h"
class Agent : public SceneNode
{
public:
    Agent(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "");

    virtual void Update(float dt);

    bool OnGround(void) const;

    void SetLeft(const float amount = 1.0f);
    void SetRight(const float amount = 1.0f);
    void SetForward(const float amount = 1.0f);
    void SetBackward(const float amount = 1.0f);
    void UnSetLeft(void);
    void UnSetRight(void);
    void UnSetForward(void);
    void UnSetBackward(void);

    void Jump(glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f));

    //void Reset(void);

protected:
    bool on_ground_;
    bool jumping_;

    float vertical_velocity_;
    float vertical_offset_;
    float gravity_;
    float fall_speed_;
    float jump_speed_;
    float step_height_;
    float movement_damping_;
    float step_offset_;
    float base_jump_speed_;
    float speed_;
    float sensitivity_;

    glm::vec3 up_;
    glm::vec3 jump_axis_;
    glm::vec3 target_position_;
    glm::vec3 prev_position_;
    glm::vec3 walk_direction_;

    glm::vec3 strafe_left_;
    glm::vec3 strafe_right_;
    glm::vec3 forward_;
    glm::vec3 backward_;

    void UpMove();
    void DownMove(float dt);
    void WalkingMove(const glm::vec3 move);

private:
    const float EPSILON = 1e-6; // For ground checking!
};
#endif