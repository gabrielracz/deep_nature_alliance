#include "tree.h"
#include "random.h"
#include "resource.h"
#include "scene_node.h"
#include "game.h"
#include "random.h"

#include <glm/ext/quaternion_trigonometric.hpp>
#include <iostream>

// Tree::Tree(std::string name, Resource* branch_geometry, Resource* branch_material, Resource* leaf_geometry, Resource* leaf_material, int branches, float height, float width, int level, int iterations, Game* game)
// : SceneNode(name, branch_geometry, branch_material), bgeometry(branch_geometry), bmaterial(branch_material), lgeometry(leaf_geometry), lmaterial(leaf_material) {

//     transform.scale = glm::vec3(width, height, width);
// }

Tree::Tree(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, float wind_offset, float wind_speed, float wind_strength, Game* game)
: SceneNode(name, mesh_id, shader_id, texture_id), wind_offset(wind_offset), wind_speed(wind_speed), wind_strength(wind_strength), game(game){
}

void Tree::Update(double dt) {
    // elapsed += dt*wind_speed;
    transform.RotateOrbit(glm::angleAxis((float)sin(wind_offset + elapsed*wind_speed) * wind_strength, glm::vec3(0.0, 0.0, 1.0)));
    SceneNode::Update(dt);
}

void GrowLeaves(SceneNode* root, Mesh* mesh, int leaves, float parent_length, float parent_width) {
    // Mesh* mesh = resman.GetMesh("Leaf");
    for(int j = 0; j < leaves; j++) {
        // position
        float woff = random<float>(0.0f, 2*PI);
        float wspd = 2.5f;
        float wstr = random<float>(0.006, 0.015);
        Tree* leaf = new Tree("Leaf", "M_Branch", "S_Default", "", woff, wspd, wstr, this);

        float p = random<float>(0.0f, parent_length/1.25f);
        float x = random<float>(0.0f, 1.0f);
        float z = random<float>(0.0f, 1.0f);
        float l = random<float>(0.5,1.0);
        float w = random<float>(0.05f, 0.1);

        float r = random<float>(PI/6, PI/3);


        leaf->transform.SetScale({w, l, w});
        leaf->transform.SetPosition({0.0f, p, 0.0f});
        // leaf->transform.position = {x, p, z};
        
        leaf->transform.SetOrbit(glm::angleAxis(random<float>(0, 2*PI), glm::vec3(0, 1, 0))); // yaw
        leaf->transform.SetOrbit(glm::angleAxis(rng.randsign() * r, glm::vec3(0, 0, 1))); // roll
        leaf->transform.SetOrbit(glm::vec3(0, -l/2, 0)); // base of the leaf
 
        root->AddChild(leaf);
        tcount++;
    }
}

void Game::GrowTree(SceneNode* root, int branches, float parent_height, float parent_width, int level, int max_iterations) {
    if(level >= max_iterations) {
        GrowLeaves(root, branches*branches, parent_height, parent_width);
        return;
    }
 
    Mesh* mesh = resman.GetMesh("Branch");
    Shader* shd = resman.GetShader("ObjectMaterial");
    level++;
    for(int j = 0; j < branches; j++) {
        // position
        float woff = 0; //rng.randfloat(0.0f, 2*PI);
        // float ws = rng.randfloat(1.0f, 2.0f);
        float wstr = rng.randfloat(0.0004, 0.001);
        float wspd = rng.randfloat(1.0, 2.0);

        Tree* branch = new Tree("Branch", "M_Branch", "S_Default", "", woff, wspd, wstr, this);

        float p = rng.randfloat(0.0f, parent_height/2.0f);
        float l = rng.randfloat(5.0f, parent_height - 1);
        float w = rng.randfloat(0.1f, parent_width/2);

        float r = rng.randfloat(PI/6, PI/3);


        branch->transform.scale = {w, l, w};
        branch->transform.position = {0.0f, p, 0.0f};
        
        branch->transform.orbit = glm::angleAxis(rng.randfloat(0, 2*PI), glm::vec3(0, 1, 0)); // yaw
        branch->transform.orbit *= glm::angleAxis(rng.randsign() * r, glm::vec3(0, 0, 1)); // roll
        branch->transform.joint = glm::vec3(0, -l/2, 0); // base of the cone
 
        root->AddChild(branch);
        GrowTree(branch, branches, l, w, level, max_iterations);
        tcount++;
    }
}

// void Game::CreateTree() {
//     Mesh* mesh = resman.GetMesh("Branch");
//     Shader* shd = resman.GetShader("ObjectMaterial");


//     int branches = 3;
//     int iterations = 4;
//     float height = rng.randfloat(10.0, 20.0);
//     float width = 0.25f;

//     float cnt = 0;
//     float spread = 20;

//     for(int i = 0; i < 2; i++) {

//         Tree* tree = new Tree("Obj_TreeTrunk", "M_Branch", "S_Default", "", 0, 0, 0, this);
//         SceneNode* root = tree;
//         GrowTree(tree, branches, height, width, 0, iterations);
//         tree->transform.position = player_position_g - glm::vec3(i*spread, 0, 40);
//         tree->transform.scale = {width, height, width};
//         scene->AddNode(tree);
//         tcount++;
//     }
//     std::cout << "SceneNodes: " << tcount << std::endl;
// }

