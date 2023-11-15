#ifndef AGENT_H_
#define AGENT_H_

#include "scene_node.h"
#include "terrain.h"

class Agent : public SceneNode
{
public:
    Agent(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "");

    virtual void Update(double dt) override;

    bool OnGround(void) const;

    void SetLeft(const float amount = 1.0f);
    void SetRight(const float amount = 1.0f);
    void SetForward(const float amount = 1.0f);
    void SetBackward(const float amount = 1.0f);
    void UnSetLeft(void);
    void UnSetRight(void);
    void UnSetForward(void);
    void UnSetBackward(void);

    void SetTerrain(Terrain* t) { terrain = t; }

    void Jump(glm::vec3 v = glm::vec3(0.0f, 0.0f, 0.0f));

    //void Reset(void);

protected:
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

    glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 jump_axis_ = glm::vec3(0.0, 1.0, 0.0f);
    glm::vec3 target_position_ = glm::vec3(0.0f);
    glm::vec3 prev_position_ = glm::vec3(0.0f);
    glm::vec3 walk_direction_ = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 forward_ = glm::vec3(0.0f);
    glm::vec3 backward_ = glm::vec3(0.0f);
    glm::vec3 strafe_left_ = glm::vec3(0.0f);
    glm::vec3 strafe_right_ = glm::vec3(0.0f);


    void UpMove();
    void DownMove(float dt);
    void WalkingMove(const glm::vec3 move);

private:
    const float EPSILON = 1e-6; // For ground checking!
    Terrain *terrain;
};
#endif