#include "collision_manager.h"
#include "transform.h"
#include "fp_player.h"

#include <glm/glm.hpp>


void CollisionManager::CheckCollisions(){
    for (auto trig : triggers){
        if(sphereToSphere(player, trig)){
            trig->ActivateTrigger();
        }
    }

    for (auto ast : asteroids){
        if (sphereToSphere(player, ast)){
            //do stuff here
        }
    }
}

void CollisionManager::AddNode(SceneNode* node){
    if (Trigger* t = dynamic_cast<Trigger*>(node)) {
        triggers.push_back(t);
    } else if (Player* p = dynamic_cast<Player*>(node)) {
        player = p;
    } else if (FP_Player *f = dynamic_cast<FP_Player*>(node)) {
     
    } else{
        asteroids.push_back(node);
    }
    // else {
    //     throw std::runtime_error("Unsupported node type in CollisionManager::AddNode");
    // }
}

bool CollisionManager::sphereToSphere(SceneNode *first, SceneNode *second) {
    glm::vec3 pos1 = first->transform.GetPosition();
    float radius1 = first->GetCollision().GetSphereRadius();
    glm::vec3 pos2 = second->transform.GetPosition();
    float radius2 = second->GetCollision().GetSphereRadius();
    return glm::distance(pos1, pos2) < radius1 + radius2;
}

bool CollisionManager::sphereToBox(SceneNode *sphereNode, SceneNode *boxNode) {
    glm::vec3 sphereCenter = sphereNode->transform.GetPosition();
    float sphereRadius = sphereNode->GetCollision().GetSphereRadius();
    glm::vec3 boxCenter = boxNode->transform.GetPosition();
    glm::vec3 boxHalfSizes = boxNode->GetCollision().GetBoxHalfSizes();

    // Compute distance from the sphere center to the box
    float distSquared = 0.f;
    for (int i = 0; i < 3; ++i) {
        float boxMin = boxCenter[i] - boxHalfSizes[i];
        float boxMax = boxCenter[i] + boxHalfSizes[i];
        if (sphereCenter[i] < boxMin) {
            distSquared += (boxMin - sphereCenter[i]) * (boxMin - sphereCenter[i]);
        } else if (sphereCenter[i] > boxMax) {
            distSquared += (sphereCenter[i] - boxMax) * (sphereCenter[i] - boxMax);
        }
    }
    return distSquared <= (sphereRadius * sphereRadius);
}

bool CollisionManager::rayToSphere(SceneNode *rayNode, SceneNode *sphereNode) {
    glm::vec3 rayOrigin = rayNode->GetCollision().GetRayOrigin();
    glm::vec3 rayDirection = rayNode->GetCollision().GetRayDirection();
    glm::vec3 sphereCenter = sphereNode->transform.GetPosition();
    float sphereRadius = sphereNode->GetCollision().GetSphereRadius();

    glm::vec3 m = rayOrigin - sphereCenter;
    float b = glm::dot(m, rayDirection);
    float c = glm::dot(m, m) - sphereRadius * sphereRadius;

    // Exit if ray's origin is outside sphere (c > 0) and ray is pointing away from sphere (b > 0)
    if (c > 0.f && b > 0.f)
        return false;
    float discr = b * b - c;
    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.f) 
        return false;

    // Ray now found to intersect sphere, compute smallest t value of intersection
    float t = -b - glm::sqrt(discr);
    // If t is negative, ray started inside sphere so clamp t to zero
    if (t < 0.f) 
        t = 0.f;
    return true;
}