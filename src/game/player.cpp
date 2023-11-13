#include <glm/ext/quaternion_trigonometric.hpp>
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
        float fuselage_length = 10.0f;
        float frontal_area = 3.0f;

        float component = 1.0f - glm::abs(glm::dot(vdir, transform.LocalAxis(FORWARD)));

        float cross_section = fuselage_length*component + frontal_area;
        float drag_constant = 2.0f;
        if(braking) {
            cross_section += 100;;
            braking = false;
        }
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

    // resistance
    float angv = glm::length(ang_velocity);
    if(angv > 0.000001) {
        glm::vec3 axis = glm::normalize(ang_velocity);

        float cross_section = 20.0f;
        if(braking) {
            cross_section += 10.0f;
            braking = false;
        }
        float resist_constant = 250.0f;
        glm::vec3 ang_resist = -axis * angv*angv * resist_constant * cross_section;
        torque += ang_resist;
    }

    ang_acceleration = glm::inverse(inertia) * torque;

    ang_velocity += ang_acceleration * (float)dt;

    float theta = glm::length(ang_velocity * (float)dt);
    if(theta > 0.0000001) {
        transform.Rotate(glm::angleAxis(theta, glm::normalize(ang_velocity)));
    }

    torque = glm::vec3(0.0f);
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

void Player::ShipControl(Controls c, float damping) {
    const float rot_force = damping * 2000.0f;
    const float thrust_force = damping * move_speed * 17550.0f;
    switch(c) {
        case Controls::THRUST:
            force += -transform.LocalAxis(FORWARD) * thrust_force;
            break;
        case Controls::BRAKE:
            braking = true;
            break;
        case Controls::PITCHD:
            torque += transform.axes[SIDE] * -rot_force;
            break;
        case Controls::PITCHU:
            torque += transform.axes[SIDE] * rot_force;
            break;
        case Controls::YAWL:
            torque += transform.axes[UP] * rot_force;
            break;
        case Controls::YAWR:
            torque += transform.axes[UP] * -rot_force;
            break;
        case Controls::ROLLL:
            torque += -transform.axes[FORWARD] * -rot_force * 2.0f;
            break;
        case Controls::ROLLR:
            torque += -transform.axes[FORWARD] * rot_force * 2.0f;
            break;
        default:
            break;
    }
}
