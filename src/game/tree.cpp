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

Tree::Tree(std::string name, Resource* geometry, Resource* material, float wind_offset, float wind_speed, float wind_strength, Game* game)
: SceneNode(name, geometry, material), wind_offset(wind_offset), wind_speed(wind_speed), wind_strength(wind_strength), game(game){

}

void Tree::Update(double dt) {
    // elapsed += dt*wind_speed;
    transform.orbit *= glm::angleAxis((float)sin(wind_offset + elapsed*wind_speed) * wind_strength, glm::vec3(0.0, 0.0, 1.0));
    SceneNode::Update(dt);
}