#include <iostream>
#include "player.h"
#include "scene_node.h"

void Player::Update(double dt) {
    transform.Pitch(angular_velocity.x);
    transform.Yaw(angular_velocity.y);
    transform.Roll(angular_velocity.z);
    transform.Translate(velocity);

    SceneNode::Update(dt);
}

void Player::Thrust(int d) {
    float speed = glm::length(velocity);
    if(d > 0 && speed < move_speed + accel) {
        velocity += -transform.LocalAxis(FORWARD) * accel;
    } else if (d < 0 && speed - accel > -move_speed - accel) { 
        velocity -= -transform.LocalAxis(FORWARD) * accel;
    }
}
