#ifndef EXPLOSION_H
#define EXPLOSION_H
#include "scene_node.h"

class Explosion : public SceneNode {
public:
   Explosion(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "")
    : SceneNode(name, mesh_id, shader_id, texture_id) {};

    virtual void Update(double dt) override;

    float timer = 7.0f;
};

#endif