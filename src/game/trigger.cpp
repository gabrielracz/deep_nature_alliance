#include "trigger.h"

void Trigger::ActivateTrigger(){
    if (action){
        action();
    } else{

    }
}

void Trigger::SuperJankDesertHeatTrigger() {
    shd->SetUniform1f(elapsed, "lastInShade");
}

// void Trigger::ActivateTrigger(){
//     action(*game);
// }

void Trigger::Update(double dt) {
    if (timerActive) {
        elapsedTime += dt;
        // Check if the timer has reached its duration
        if (elapsedTime >= timerDuration) {
            ActivateTrigger();
        }
    }

    // Other update logic
    SceneNode::Update(dt);
}