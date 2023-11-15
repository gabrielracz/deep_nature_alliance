#include "collidable.h"


void Collidable::Collide(Collidable *other){
    other->transform.SetPosition(closestPossiblePos(other));
}

glm::vec3 Collidable::closestPossiblePos(Collidable* other) {
    
}

// void Trigger::ActivateTrigger(){
//     action(*game);
// }

// void Collidable::Update(double dt) {
//     //stuff
//     SceneNode::Update(dt);
// }