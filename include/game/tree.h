#ifndef TREE_H
#define TREE_H
#include "scene_node.h"
#include "random.h"

class Game;

class Tree : public SceneNode {
public:

    Tree(const std::string name, const std::string& mesh_id,
         const std::string shader_id, const std::string& texture_id,
         float wind_speed, float wind_offset, float wind_strength, Game* game);
    // : SceneNode(name, geometry, material){}

    virtual void Update(double dt) override;

    static std::string branch_mesh;
    static std::string branch_texture;
    static std::string branch_normal_map;
    static std::string leaf_mesh;
    static std::string leaf_texture;
    static std::string leaf_normal_map;
    void GrowTree();
private:
    void GrowBranches(SceneNode* root, int branches, float parent_height, float parent_width, int level, int max_iterations);
    void GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width);

    float wind_speed = 0.5f;
    float wind_offset;
    float wind_strength;

    RandGenerator rand = {101181};

    Game* game;
};

#endif