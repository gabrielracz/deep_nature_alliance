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

    for (SceneNode * n : blockingCollision){
        if (sphereToSphere(player, n)){
            std::cout << "hit" << std::endl;
            player->transform.ResetPosition();
        }
    }


    for (auto toggle : toggles) {
        //NOTE: we are getting collision hit on spawn in (weird???)
        if(GetCollisionRaw(toggle, player)) {
            if(!toggle->GetToggle()) {
                toggle->ToggleOn(player);
            }
        } else if (toggle->GetToggle() && toggle->Triggered()) {
            toggle->ToggleOff(player);
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
                game->SpawnExplosion(apos, glm::vec3(4.0f));
               game->ShipHitPlanet({0.0f,0.0f,0.0f});
                asteroid->DeleteInstance(i);
            }

            for(auto it = rockets.begin(); it != rockets.end();) {
                SceneNode* rocket = *it;
                float rocket_rad = glm::length(rocket->transform.GetScale());
                if(rocket->deleted) {
                    it = rockets.erase(it);
                } else if(glm::length(rocket->transform.GetPosition() - apos) < rad + rocket_rad) {
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

    for (auto it = othercollideables.begin(); it != othercollideables.end();) {
        SceneNode* other = *it;
        if(GetCollision(other, player) && other->GetCollider()->oneoff) {
            it = othercollideables.erase(it);
            std::cout << "deleted" << std::endl;
        } else {
            it++;
        }
    }
}

bool CollisionManager::GetCollision(SceneNode* obj1, SceneNode* obj2) {
    if(obj1->deleted || obj2->deleted) {
        //probably should delete here weird glitch
        return false;
    }

    Collider* col = obj1->GetCollider();
    Collider* other = obj2->GetCollider();
    if (col && other) {
        if (other->CollidesWith(col)) {
            // Backwards because visitor pattern
            col->invokeCallback(obj2);
            return true;
        } 
        if (col->CollidesWith(other)) {
            // Backwards because visitor pattern
            other->invokeCallback(obj1);
            return true;
        }
    }
    return false;
}


bool CollisionManager::GetCollisionRaw(SceneNode* obj1, SceneNode* obj2)
{
    if(obj1->deleted || obj2->deleted) {
        return false;
    }

    Collider* col = obj1->GetCollider();
    Collider* other = obj2->GetCollider();
    if (col && other) {
        if (other->CollidesWith(col)) {
            return true;
        } 
        if (col->CollidesWith(other)) {
            return true;
        }
    }
    return false;
}

void CollisionManager::AddNode(SceneNode* node){

    switch(node->GetNodeType()) {
        case TTRIGGER:
            triggers.push_back(dynamic_cast<Trigger*>(node));
            break;
        case TDONTUSECOLLIDER:
            blockingCollision.push_back(node);
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
            break;
        case TTOGGLE:
            toggles.push_back(dynamic_cast<Toggle*>(node));
        default:
            othercollideables.push_back(node);
            break;
    }
}

//only does instanced collision on second node becuase fuck you its already ugly enough
bool CollisionManager::sphereToSphere(SceneNode *first, SceneNode *second) {
    glm::vec3 pos1 = first->transform.GetPosition();
    float radius1 = first->GetCollision().GetSphereRadius();

    if (second->GetNumInstances() > 0){
        for (Transform& t : second->GetInstances()){
            glm::vec3 pos2 = t.GetPosition();
            float radius2 = second->GetCollision().GetSphereRadius();

            if (glm::distance(pos1, pos2) < radius1 + radius2){
                return true;
            }
        }
    } else{
        glm::vec3 pos2 = second->transform.GetPosition();
        float radius2 = second->GetCollision().GetSphereRadius();

        return glm::distance(pos1, pos2) < radius1 + radius2;
    }
    return false;
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
    // delete player;
}