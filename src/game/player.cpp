#include <iostream>
#include "player.h"
#include "scene_node.h"
#include "transform.h"
#include "glm/gtx/string_cast.hpp"

#define DRAG_CONSTANT 2.0f

void Player::Update(double dt) {

    acceleration = glm::vec3(0.0f);
    ang_acceleration = glm::vec3(0.0f);

    // AIR RESISTANCE
	// Proptional to v^2 * some constants such as fluid density and area...
	// Force applied in the opposite direction to velocity
	float v = glm::length(velocity);
	if (glm::length(velocity) > 0.0f) {
        glm::vec3 vdir = glm::normalize(velocity);
        float vmag = glm::length(velocity);
        float drag_constant = 2.0f;
        float fuselage_length = 10.0f;
        float cross_section = fuselage_length - fuselage_length*glm::dot(vdir, transform.LocalAxis(FORWARD)) + 3.0f;
        glm::vec3 fuselage_drag = -vdir * vmag*vmag * drag_constant * cross_section;
        force += fuselage_drag;

    }

	// Velocity is accumulated while acceleration denotes our rate of change of velocity
	// at this exact time step. acceleration = dv/dt
    acceleration += force / mass;

	velocity += acceleration * (float)dt;
	transform.position += velocity * (float)dt;

    transform.Pitch(angular_velocity.x);
    transform.Yaw(angular_velocity.y);
    transform.Roll(angular_velocity.z);
    // transform.Translate(velocity);

    force = glm::vec3(0.0f);
    SceneNode::Update(dt);
}

void Player::Thrust(int d) {
    float speed = glm::length(velocity);
    if(d > 0 && speed < move_speed + accel_amt) {
        velocity += -transform.LocalAxis(FORWARD) * accel_amt;
    } else if (d < 0 && speed - accel_amt > -move_speed - accel_amt) { 
        velocity -= -transform.LocalAxis(FORWARD) * accel_amt;
    }
}

void Player::ShipControl(Controls c) {
    switch(c) {
        case Controls::THRUST:
            force += -transform.LocalAxis(FORWARD) * move_speed * 7550.0f;
            break;
        case Controls::BRAKE:
            force += transform.LocalAxis(FORWARD) * move_speed * 250.0f;
            break;
        case Controls::PITCH:
            break;
        case Controls::YAW:
            break;
        case Controls::ROLL:
            break;
        default:
            break;
    }
}
