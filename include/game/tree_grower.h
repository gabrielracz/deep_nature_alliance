#include "defines.h"
#include "scene_node.h"

int tcount = 0;
void GrowLeaves(SceneNode* root, Mesh* mesh, int leaves, float parent_length, float parent_width) {
    // Mesh* mesh = resman.GetMesh("Leaf");
    for(int j = 0; j < leaves; j++) {
        // position
        float woff = rng.randfloat(0.0f, 2*PI);
        float wspd = 2.5f;
        float wstr = rng.randfloat(0.006, 0.015);
        Tree* leaf = new Tree("Leaf", "M_Branch", "S_Default", "", woff, wspd, wstr, this);

        float p = rng.randfloat(0.0f, parent_length/1.25f);
        float x = rng.randfloat(0.0f, 1.0f);
        float z = rng.randfloat(0.0f, 1.0f);
        float l = rng.randfloat(0.5,1.0);
        float w = rng.randfloat(0.05f, 0.1);

        float r = rng.randfloat(PI/6, PI/3);


        leaf->transform.SetSca = {w, l, w};
        leaf->transform.SetPosition({0.0f, p, 0.0f});
        // leaf->transform.position = {x, p, z};
        
        leaf->transform.orbit = glm::angleAxis(rng.randfloat(0, 2*PI), glm::vec3(0, 1, 0)); // yaw
        leaf->transform.orbit *= glm::angleAxis(rng.randsign() * r, glm::vec3(0, 0, 1)); // roll
        leaf->transform.joint = glm::vec3(0, -l/2, 0); // base of the leaf
 
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
