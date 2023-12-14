#include "tree.h"
#include "random.h"
#include "resource.h"
#include "scene_node.h"
#include "game.h"
#include "random.h"

#include <glm/ext/quaternion_trigonometric.hpp>
#include <iostream>

std::string Tree::branch_mesh;
std::string Tree::branch_texture;
std::string Tree::branch_normal_map;
std::string Tree::leaf_mesh;
std::string Tree::leaf_texture;
std::string Tree::leaf_normal_map;

// Tree::Tree(std::string name, Resource* branch_geometry, Resource* branch_material, Resource* leaf_geometry, Resource* leaf_material, int branches, float height, float width, int level, int iterations, Game* game)
// : SceneNode(name, branch_geometry, branch_material), bgeometry(branch_geometry), bmaterial(branch_material), lgeometry(leaf_geometry), lmaterial(leaf_material) {

//     transform.scale = glm::vec3(width, height, width);
// }

Tree::Tree(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, 
           float wind_speed, float wind_offset, float wind_strength, Game* game)
: SceneNode(name, mesh_id, shader_id, texture_id), game(game),
  wind_speed(wind_speed), wind_offset(wind_offset), wind_strength(wind_strength) {
}

void Tree::Update(double dt) {
    float angle = (float)sin(wind_offset + elapsed*wind_speed) * wind_strength;
    transform.RotateOrbit(glm::angleAxis(angle, glm::vec3(0.0, 0.0, 1.0)));
    SceneNode::Update(dt);
}

void Tree::GrowLeaves(SceneNode* root, int leaves, float parent_length, float parent_width) {
    // Mesh* mesh = resman.GetMesh("Leaf");
    for(int j = 0; j < leaves; j++) {
        // position
        float woff = rand.randfloat(0.0f, 2*PI);
        float wspd = 2.5f;
        float wstr = rand.randfloat(0.006, 0.015);
        Tree* leaf = new Tree("Leaf", Tree::leaf_mesh, shader_id, Tree::leaf_texture, wspd, woff, wstr, game);
        leaf->SetNormalMap(Tree::leaf_normal_map);

        float p = rand.randfloat(0.0f, parent_length/1.25f);
        float x = rand.randfloat(0.0f, 10.0f);
        float z = rand.randfloat(0.0f, 10.0f);
        float l = rand.randfloat(1.0,2.0);
        float w = rand.randfloat(1.0f, 3.0);

        float r = rand.randfloat(PI/6, PI/3);


        leaf->transform.SetScale({w, l, w});
        leaf->transform.SetPosition({x, p, z});
        // leaf->transform.SetPosition({0.0f, 3.0, 0.0f});
        // leaf->transform.position = {x, p, z};
        
        glm::quat orbit = glm::angleAxis(rand.randfloat(0, 2*PI), glm::vec3(0, 1, 0)); // yaw
        orbit *= glm::angleAxis(rand.randsign() * r, glm::vec3(0, 0, 1)); // roll

        leaf->transform.SetOrbit(orbit); // yaw
        leaf->transform.SetJoint(glm::vec3(0, -l/2, 0)); // base of the leaf
        leaf->transform.Roll(rand.randfloat(0.0, PI*2.0f));
 
        root->AddChild(leaf);
    }
}

void Tree::GrowBranches(SceneNode* root, int branches, float parent_height, float parent_width, int level, int max_iterations) {
    if(level >= max_iterations) {
        GrowLeaves(root, 6, parent_height, parent_width);
        return;
    }

    level++;
    float bonus_branches = level == 0 ? branches : 0;
    for(int j = 0; j < branches + bonus_branches; j++) {
        // position
        float woff = 0; //rand.randfloat(0.0f, 2*PI);
        // float ws = rand.randfloat(1.0f, 2.0f);
        float wstr = rand.randfloat(0.0004, 0.001);
        float wspd = rand.randfloat(1.0, 2.0);

        // Tree* branch = new Tree("Branch", "M_Branch", "S_Default", "", woff, wspd, wstr, this);
        Tree* branch = new Tree("Branch", Tree::branch_mesh, shader_id, Tree::branch_texture, wspd, woff, wstr, game);
        branch->SetNormalMap(Tree::branch_normal_map);

        float p = rand.randfloat(0.0f, parent_height/2.0f);
        float l = rand.randfloat(parent_height/4.0f, parent_height/1.2);
        float w = rand.randfloat(0.5f, parent_width/2);

        float r = rand.randfloat(PI/6, PI/3);


        branch->transform.SetScale({w, l, w});
        branch->transform.SetPosition({0.0f, p, 0.0f});
        branch->material = root->material;
        branch->material.texture_repetition = l/2.0f;
        // branch->transform.SetPosition({0.0f, 3.0, 0.0f});

        float region_border = 0.0f;
        float region_size = PI*2/branches;
        float region_start = region_size*j + region_border;
        float angle_move = rand.randfloat(-region_size/3, region_size/3);

        glm::quat orbit = glm::angleAxis(region_start + angle_move, glm::vec3(0, 1, 0)); // yaw
        orbit *= glm::angleAxis(rand.randsign() * r, glm::vec3(0, 0, 1)); // roll
        
        branch->transform.SetOrbit(orbit);
        branch->transform.SetJoint(glm::vec3(0, -l/2, 0)); // base of the cone
 
        root->AddChild(branch);
        GrowBranches(branch, branches, l, w, level, max_iterations);
    }
}

void Tree::GrowTree() {
    int branches = 3;
    int iterations = 3;
    float height = rand.randfloat(45.0, 60.0);
    float width = 2.0f;
    transform.SetScale({width, height, width});
    transform.Translate({0, height/2, 0});
    SetNormalMap(Tree::branch_normal_map);
    GrowBranches(this, branches, height, width, 0, iterations);
}


