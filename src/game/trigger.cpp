#include "trigger.h"

void Trigger::ActivateTrigger() {
    if (action){
        action();
    } else{
        SuperJankDesertHeatTrigger();
    }
}

void Trigger::SuperJankDesertHeatTrigger() {
    shd->Use();
    shd->SetUniform1f(elapsed, "lastInShade");
}

void Trigger::SetShader(Shader* s) {
    shd = s;
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

    if (shd){
        shd->Use();
        shd->SetUniform1f(elapsed, "timerJank");
    }

    // Other update logic
    SceneNode::Update(dt);
}