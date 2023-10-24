#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "enemy.h"

void Enemy::Update(double dt) {
    glm::vec3 target_dir = target->position - transform.position;
    float distance = glm::length(target_dir);
    if( distance > 0.01f && distance < 200.0f) {
        velocity = glm::normalize(target_dir) * move_speed;
    }

    transform.position += velocity;
}