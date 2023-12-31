#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_PLAYER_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_PLAYER_H_

#include "scene_node.h"

class Player : public SceneNode{
public:
    enum class Controls{
        W,
        A,
        S,
        D,
        SHIFT,
        CTRL,
        Q,
        E,
        SPACE,
        UP,
        DOWN,
        LEFTCLICK
    };

    Player(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id = "")
    : SceneNode(name, mesh_id, shader_id, texture_id){}

    virtual void Control(Controls c, float dt, float damping = 1.0) = 0;
    virtual void MouseControls(Mouse& mouse) = 0;

    void ResetPosition(){ transform.SetPosition(prev_position_); }


    glm::vec3 velocity         = {0.0f, 0.0f, 0.0f};
    glm::vec3 ang_velocity = {0.0f, 0.0f, 0.0f};

    void SetStatic(bool stasis) { player_static_ = stasis; }
    void UnlockDash() {dash_unlocked_ = true;}
    void SetLimpMode(bool l) {limp_mode = l;}

    bool dash_unlocked_ = false;
    bool limp_mode = false;
protected:

    bool player_static_ = false;

    glm::vec3 prev_position_ = glm::vec3(0.0f);

};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_PLAYER_H_