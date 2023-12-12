#include "rocket.h"
#include "game.h"

void Rocket::Update(double dt) {
    if(elapsed > fuse_timer) {
        deleted = true;
        game->SpawnExplosion(transform.GetPosition(), glm::vec3(1.0f));
        return;
    }

    velocity += direction * acceleration * (float)dt;
    transform.TranslateRelative(velocity * (float)dt);
    SceneNode::Update(dt);
}

void Rocket::AddThrust(Thrust *t) {
    thrust = t;
    thrust->amount = 1.0f;
    children.push_back(t);
}