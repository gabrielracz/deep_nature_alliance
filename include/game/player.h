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
        SPACE
    };

    Player(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "")
    : SceneNode(name, mesh_id, shader_id, texture_id) {}

    virtual void Control(Controls c, float damping = 1.0) = 0;
    virtual void MouseControls(Mouse& mouse) = 0;

    glm::vec3 velocity         = {0.0f, 0.0f, 0.0f};

};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_PLAYER_H_