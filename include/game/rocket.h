#ifndef ROCKET_H
#define ROCKET_H
#include "scene_node.h"
#include "thrust.h"
class Game;

class Rocket : public SceneNode {
public:
    Rocket(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "", Game* game = nullptr)
    : SceneNode(name, mesh_id, shader_id, texture_id), game(game) {};

    virtual void Update(double dt) override;
    void AddThrust(Thrust* thrust);

    Thrust* thrust;
    glm::vec3 direction {0.0, 0.0, -1.0};
    glm::vec3 velocity {};
    float acceleration = 200.0f;
    float fuse_timer = 4.0f;
    Game* game = nullptr;
};

#endif