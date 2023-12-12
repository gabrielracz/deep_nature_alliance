#include "explosion.h"

void Explosion::Update(double dt) {
    if(elapsed > timer) {
        deleted = true;
        return;
    }
    SceneNode::Update(dt);
}