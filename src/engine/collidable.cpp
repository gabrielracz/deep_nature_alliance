#include "collidable.h"

void Collidable::ActivateTrigger(){
    action();
}

void Collidable::Collide(Collidable *other){
    action();
    collisionAction(other);
}

// void Trigger::ActivateTrigger(){
//     action(*game);
// }

// void Collidable::Update(double dt) {
//     //stuff
//     SceneNode::Update(dt);
// }