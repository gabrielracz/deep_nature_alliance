#include "rocket.h"
#include "game.h"

void Rocket::Update(double dt) {
    if(elapsed > fuse_timer) {
        deleted = true;
        game->SpawnExplosion(transform.GetPosition(), glm::vec3(1.0f));
        return;
    }

    velocity += transform.GetOrientation() * direction * acceleration * (float)dt;
    transform.Translate(velocity * (float)dt);
    SceneNode::Update(dt);
}

void Rocket::AddThrust(Thrust *t) {
    thrust = t;
    thrust->amount = 1.0f;
    AddChild(t);
}