#include "explosion.h"

void Explosion::Update(double dt) {
    if(elapsed > timer) {
        deleted = true;
    }
    SceneNode::Update(dt);
}