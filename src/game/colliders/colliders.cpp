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

bool TerrainCollider::CollidesWithTerrain(TerrainCollider* other) {
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
    glm::vec3 position = player_.transform.GetPosition();
    Terrain& t = other->GetTerrain();
    float height = t.SampleHeight(position.x, position.z);
    bool collided = height > position.y - player_.GetHeight() - player_.GetVerticalStep();
    //printf("Terrain: %f Player: %f Collided: %s \n",height,position.y - player_.GetHeight() - player_.GetVerticalStep(), t.IsDeathTerrain() ? "true" : "false");
    if (collided && t.IsDeathTerrain()) {
        printf("We ded! \n");
        player_.Reset();
    }
    return false;
}

bool FPPlayerCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return false;
}

