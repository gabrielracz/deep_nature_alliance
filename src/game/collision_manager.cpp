#include "collision_manager.h"
#include "transform.h"
#include "fp_player.h"
#include "colliders/colliders.h"
#include "game.h"

#include <glm/glm.hpp>


void CollisionManager::CheckCollisions(){
    for (auto trig : triggers){
        if(sphereToSphere(player, trig)){
            trig->ActivateTrigger();
        }
    }

    for (auto asteroid : asteroids){
        // GetCollision(asteroid, player);
        int i = 0;
        for(Transform& a : asteroid->GetInstances()) {
            float rad = glm::length(a.GetScale());
            float player_rad = glm::length(player->transform.GetScale());
            glm::vec3 apos = asteroid->transform.GetPosition() + a.GetPosition();
            if(glm::length(player->transform.GetPosition() - apos) < rad + player_rad) {
                // collision
               game->ShipHitPlanet({0.0f,0.0f,0.0f});
               return;
            }

            for(auto it = rockets.begin(); it != rockets.end();) {
                SceneNode* rocket = *it;
                float rocket_rad = glm::length(rocket->transform.GetScale());
                if(glm::length(rocket->transform.GetPosition() - apos) < rad + rocket_rad) {
                    game->SpawnExplosion(rocket->transform.GetPosition(), glm::vec3(1.0f));
                    game->SpawnExplosion(apos, glm::vec3(4.0f));
                    rocket->deleted = true;
                    asteroid->DeleteInstance(i);
                    it = rockets.erase(it);
                } else {
                    ++it;
                }
            }
            i++;
        }
    }

    for (auto item : items) {
        GetCollision(item, player);
    }

    for (auto beacon : beacons) {
        GetCollision(beacon, player);
    }

    for (auto other : othercollideables) {
        GetCollision(other, player);
    }
}

void CollisionManager::GetCollision(SceneNode* obj1, SceneNode* obj2) {
    Collider* col = obj1->GetCollider();
    Collider* other = obj2->GetCollider();
    if (col && other) {
        if (other->CollidesWith(col)) {
            // Backwards because visitor pattern
            col->invokeCallback(obj2);
        } 
        if (col->CollidesWith(other)) {
            // Backwards because visitor pattern
            other->invokeCallback(obj1);
        }
    }
}

void CollisionManager::AddNode(SceneNode* node){

    switch(node->GetNodeType()) {
        case TTRIGGER:
            triggers.push_back(dynamic_cast<Trigger*>(node));
            break;
        case TITEM:
            items.push_back(node);
            break;
        case TBEACON:
            beacons.push_back(node);
            break;
        case TASTEROID:
            asteroids.push_back(node);
            break;
        case TPLAYER:
            player = node;
            break;
        case TSHIP:
            player = node;
            break;
        case TROCKET:
            rockets.push_back(node);
        default:
            othercollideables.push_back(node);
            break;
    }
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

void CollisionManager::Reset()
{
    triggers.clear();
    items.clear();
    asteroids.clear();
    beacons.clear();
    othercollideables.clear();
    delete player;
}