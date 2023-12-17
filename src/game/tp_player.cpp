#include <glm/ext/quaternion_trigonometric.hpp>
#include <iostream>
#include <memory>
#include "tp_player.h"
#include "scene_node.h"
#include "transform.h"
#include "glm/gtx/string_cast.hpp"
#include "colliders/colliders.h"
#include "game.h"

#define DRAG_CONSTANT 2.0f


Tp_Player::Tp_Player(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id, Game* game)
    : Player(name, mesh_id, shader_id, texture_id), game(game)
{
    //c->Attach(&transform); 
    SphereCollider* col = new SphereCollider(*this, collider_radius_);
    col->SetCallback([this](SceneNode& other) { this->HandleCollisionWith(&other); });
    SetCollider(col);
    SetNodeType(TSHIP);
}

void Tp_Player::Update(double dt) {

    //printf("X: %f Y: %f Z: %f\n", transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z);

    acceleration = glm::vec3(0.0f);
    ang_acceleration = glm::vec3(0.0f);

    // AIR RESISTANCE
	// Proptional to v^2 * some constants such as fluid density and area...
	// Force applied in the opposite direction to velocity
	float v = glm::length(velocity);
	if (glm::length(velocity) > 0.0f) {
        glm::vec3 vdir = glm::inverse(transform.GetOrientation()) * glm::normalize(velocity); // go from velocity in local space to model space (base axis)
        float vmag = glm::length(velocity);
        float fuselage_length = 10.0f;
        float frontal_area = 2.0f;

        float component = 1.0f - glm::abs(glm::dot(vdir, transform.GetAxis(FORWARD)));

        float cross_section = fuselage_length*component + frontal_area;

        float drag_constant = 1.0f;
        if(braking) {
            cross_section *= 4;;
            braking = false;
        }
        glm::vec3 fuselage_drag = -vdir * vmag*vmag * drag_constant * cross_section;
        force += fuselage_drag;
    }

	// Velocity is accumulated while acceleration denotes our rate of change of velocity
	// at this exact time step. acceleration = dv/dt
    acceleration += transform.GetOrientation() * force / mass; // force is in model space
	velocity += acceleration * (float)dt;
	transform.Translate(velocity * (float)dt);

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
        float resist_constant = 150.0f;
        glm::vec3 ang_resist = -axis * angv*angv * resist_constant * cross_section;
        torque += ang_resist;
    }

    ang_acceleration = glm::inverse(inertia) * torque;

    ang_velocity += ang_acceleration * (float)dt;

    float theta = glm::length(ang_velocity * (float)dt);
    if(theta > 0.0000001) {
        transform.Rotate(glm::angleAxis(theta, glm::normalize(ang_velocity)));
    }

    transform.Pitch(angular_velocity.x);
    transform.Yaw(angular_velocity.y);
    transform.Roll(angular_velocity.z);

    torque = glm::vec3(0.0f);

    ConfigureThrusters(dt);
    thrusting = false;
    SceneNode::Update(dt);
}

void Tp_Player::MouseControls(Mouse& mouse){
    float mouse_sens = -0.1f;
	glm::vec2 look = mouse.move * mouse_sens;

    Control(Player::Controls::Q, look.x);
    Control(Player::Controls::E, look.y);
}

void Tp_Player::Control(Controls c, float dt, float damping){
    const float rot_force = damping * 2000.0f;
    float thrust_force = damping * move_speed * 28550.0f;
    if(limp_mode) {
        thrust_force *= 0.3;
    }
    switch(c) {
        case Player::Controls::SHIFT:
            // f;worce += -transform.LocalAxis(FORWARD) * thrust_force;
            force += transform.GetAxis(FORWARD) * thrust_force;
            thrusting = true;
            break;
        case Player::Controls::CTRL:
            braking = true;
            break;
        case Player::Controls::W:
            torque += transform.GetAxis(SIDE) * -rot_force;
            break;
        case Player::Controls::S:
            torque += transform.GetAxis(SIDE) * rot_force;
            break;
        case Player::Controls::Q:
            torque += transform.GetAxis(UP) * rot_force;
            break;
        case Player::Controls::E:
            torque += transform.GetAxis(UP) * -rot_force;
            break;
        case Player::Controls::A:
            torque += transform.GetAxis(FORWARD) * -rot_force * 2.0f;
            break;
        case Player::Controls::D:
            torque += transform.GetAxis(FORWARD) * rot_force * 2.0f;
            break;
        case Player::Controls::SPACE:{
            if(limp_mode) {return;}
            glm::vec3 forward_vel = transform.GetOrientation() * glm::vec3(0.0, 0.0, (glm::inverse(transform.GetOrientation()) * velocity).z);
            game->SpawnRocket(transform.GetPosition() + transform.GetOrientation() * glm::vec3(0.0, -0.25, 0.0), transform.GetOrientation(), forward_vel);
            break;}
        default:
            break;
    }
}

void Tp_Player::ConfigureThrusters(double dt) {
    float thrst = 1.0 * dt;
    if(thrusting) {
        if(!limp_mode) {
            thrust1->ChangeAmount(thrst);
        }
        thrust2->ChangeAmount(thrst);
        thrust3->ChangeAmount(thrst);
    } else {
        if(!limp_mode) {
            thrust1->ChangeAmount(-thrst);
        }
        thrust2->ChangeAmount(-thrst);
        thrust3->ChangeAmount(-thrst);
    }
}


void Tp_Player::SetUniforms(Shader *shader, const glm::mat4 &view_matrix, const glm::mat4& parent_matrix) {
    if(thrust1) {
        thrust1->SetUniforms(shader, view_matrix);
    }
    if(thrust2) {
        thrust2->SetUniforms(shader, view_matrix);
    }
    SceneNode::SetUniforms(shader, view_matrix, parent_matrix);
}