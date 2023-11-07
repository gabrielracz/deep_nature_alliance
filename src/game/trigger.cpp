#include "trigger.h"

void Trigger::ActivateTrigger(){
    action();
}

// void Trigger::ActivateTrigger(){
//     action(*game);
// }

void Trigger::Update(double dt) {
    //stuff
    SceneNode::Update(dt);
}