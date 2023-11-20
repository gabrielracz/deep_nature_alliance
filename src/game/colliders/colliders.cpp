#include "colliders/colliders.h"

// BOX COLLIDER
bool BoxCollider::CollidesWithBox(BoxCollider* other) {
    return false;
}

bool BoxCollider::CollidesWithSphere(SphereCollider* other) {
    return false;
}

bool BoxCollider::CollidesWithTerrain(TerrainCollider* other) {
    return false;
}

bool BoxCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return false;
}


// SPHERE COLLIDER
bool SphereCollider::CollidesWithBox(BoxCollider* other) {
    return false;
}

bool SphereCollider::CollidesWithSphere(SphereCollider* other) {
    return false;
}

bool SphereCollider::CollidesWithTerrain(TerrainCollider* other) {
    return false;
}

bool SphereCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return false;
}

// TERRAIN COLLIDER
bool TerrainCollider::CollidesWithBox(BoxCollider* other) {
    return false;
}

bool TerrainCollider::CollidesWithSphere(SphereCollider* other) {
    return false;
}

bool TerrainCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return false;
}

// PLAYER COLLIDER
bool FPPlayerCollider::CollidesWithBox(BoxCollider* other) {
    return false;
}

bool FPPlayerCollider::CollidesWithSphere(SphereCollider* other) {
    return false;
}

bool FPPlayerCollider::CollidesWithTerrain(TerrainCollider* other) {
    return false;
}

bool FPPlayerCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return false;
}

