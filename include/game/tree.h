#ifndef TREE_H
#define TREE_H
#include "scene_node.h"

class Game;

class Tree : public SceneNode {
public:

    Tree(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, float wind_offset, float wind_speed, float wind_strength, Game* game);
    // : SceneNode(name, geometry, material){}

    virtual void Update(double dt) override;
private:
    float wind_speed;
    float wind_offset;
    float wind_strength;
    Resource* bgeometry;
    Resource* bmaterial;
    Resource* lgeometry;
    Resource* lmaterial;
    Game* game;
};
#endif