#include "colliders/colliders.h"

// BOX COLLIDER
bool BoxCollider::CollidesWithBox(BoxCollider* other) {
    return false;
}

bool BoxCollider::CollidesWithSphere(SphereCollider* other) {

    bool collided = SphereCollision(other->GetOwner(), other->GetRadius());
    if (collided) {
        owner_.HandleCollisionWithSceneNode(other->GetOwner());
    }
    return collided;
}

bool BoxCollider::CollidesWithTerrain(TerrainCollider* other) {
    return false;
}

bool BoxCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    bool collided = SphereCollision(other->GetPlayer(), other->GetRadius());
    if (collided) {
        owner_.HandleCollisionWithSceneNode(other->GetPlayer());
    }
    return collided;
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
    bool collided = height > position.y + player_.GetHeight() + player_.GetVerticalStep();
    if (collided && t.IsDeathTerrain()) {
        player_.Reset();
    }
    return false;
}

bool FPPlayerCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    return false;
}

