#include "colliders/colliders.h"

// BOX COLLIDER
bool BoxCollider::CollidesWithBox(BoxCollider* other) {
    return false;
}

bool BoxCollider::CollidesWithSphere(SphereCollider* other) {

    return SphereCollision(other->GetOwner(), other->GetRadius());
}

bool BoxCollider::CollidesWithTerrain(TerrainCollider* other) {
    return false;
}

bool BoxCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return SphereCollision(other->GetPlayer(), other->GetRadius());
}

bool BoxCollider::SphereCollision(SceneNode& collider, float radius) {
    glm::vec3 sphereCenter = collider.transform.GetWorldPosition();
    glm::vec3 boxCenter = owner_.transform.GetWorldPosition();
    glm::vec3 boxHalfSizes = box_half_sizes_;
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

    return distSquared <= (radius * radius);
}


// SPHERE COLLIDER
bool SphereCollider::CollidesWithBox(BoxCollider* other) {
    return false;
}

bool SphereCollider::CollidesWithSphere(SphereCollider* other) {
    glm::vec3 pos1 = owner_.transform.GetPosition();
    glm::vec3 pos2 = other->GetOwner().transform.GetWorldPosition();
    return glm::distance(pos1, pos2) < radius_ + other->GetRadius();
}

bool SphereCollider::CollidesWithTerrain(TerrainCollider* other) {
    return false;
}

bool SphereCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    glm::vec3 pos1 = owner_.transform.GetPosition();
    glm::vec3 pos2 = other->GetPlayer().transform.GetWorldPosition();
    return glm::distance(pos1, pos2) < radius_ + other->GetRadius();
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
    glm::vec3 pos1 = player_.transform.GetPosition();
    glm::vec3 pos2 = other->GetOwner().transform.GetWorldPosition();
    return glm::distance(pos1, pos2) < radius_ + other->GetRadius();
}

bool FPPlayerCollider::CollidesWithTerrain(TerrainCollider* other) {
    glm::vec3 position = player_.transform.GetPosition();
    Terrain& t = other->GetTerrain();
    if (!t.SampleOn(position.x, position.z)) {
        return false;
    }
    float height = t.SampleHeight(position.x, position.z);
    return height > position.y + player_.GetHeight() + player_.GetVerticalStep();
}

bool FPPlayerCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return false;
}

