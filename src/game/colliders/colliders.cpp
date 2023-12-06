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
    glm::vec3 pos2 = other->GetOwner().transform.GetPosition();
    return glm::distance(pos1, pos2) < GetRadius() + other->GetRadius();
}

bool SphereCollider::CollidesWithTerrain(TerrainCollider* other) {
    return false;
}

bool SphereCollider::CollidesWithPlayer(FPPlayerCollider* other) {
    glm::vec3 pos1 = owner_.transform.GetPosition();
    glm::vec3 pos2 = other->GetPlayer().transform.GetWorldPosition();
    return glm::distance(pos1, pos2) < GetRadius() + other->GetRadius();
}

// TERRAIN COLLIDER

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

bool FPPlayerCollider::CollidesWithCylinder(CylinderCollider *other) {
    glm::vec3 sphere_position = player_.transform.GetPosition();
    glm::vec3 cylinder_position = other->GetOwner().transform.GetPosition();
    float closestPointOnAxis = glm::clamp(sphere_position.y, cylinder_position.y, cylinder_position.y + other->GetHeight());
    float distanceSquared = glm::distance2(glm::vec3(sphere_position.x, closestPointOnAxis, sphere_position.z), cylinder_position);
    return distanceSquared <= (radius_ + other->GetRadius()) * (radius_ + other->GetRadius());
}

bool FPPlayerCollider::CollidesWithScalingCylinder(ScalingCylinderCollider *other) {
    glm::vec3 sphere_position = player_.transform.GetPosition();
    glm::vec3 cylinder_position = other->GetOwner().transform.GetPosition();
    float height = other->GetOwner().transform.GetScale().y;
    float radius = other->GetOwner().transform.GetScale().x * 0.5f;
    float closestPointOnAxis = glm::clamp(sphere_position.y, cylinder_position.y, cylinder_position.y + height);
    float distanceSquared = glm::distance2(glm::vec3(sphere_position.x, closestPointOnAxis, sphere_position.z), cylinder_position);
    return distanceSquared <= (radius + radius_) * (radius + radius_);
}

//CYLINDER COLLIDER
bool CylinderCollider::CollidesWithPlayer(FPPlayerCollider *other) {
    glm::vec3 sphere_position = other->GetPlayer().transform.GetPosition();
    glm::vec3 cylinder_position = owner_.transform.GetPosition();
    float closestPointOnAxis = glm::clamp(sphere_position.y, cylinder_position.y, cylinder_position.y + height_);
    float distanceSquared = glm::distance2(glm::vec3(sphere_position.x, closestPointOnAxis, sphere_position.z), cylinder_position);
    return distanceSquared <= (radius_ + other->GetRadius()) * (radius_ + other->GetRadius());
}
bool CylinderCollider::CollidesWithSphere(SphereCollider *other) {
    glm::vec3 sphere_position = other->GetOwner().transform.GetPosition();
    glm::vec3 cylinder_position = owner_.transform.GetPosition();
    float closestPointOnAxis = glm::clamp(sphere_position.y, cylinder_position.y, cylinder_position.y + height_);
    float distanceSquared = glm::distance2(glm::vec3(sphere_position.x, closestPointOnAxis, sphere_position.z), cylinder_position);
    return distanceSquared <= (radius_ + other->GetRadius()) * (radius_ + other->GetRadius());
}

// SCALING CYLINDER COLLIDER
bool ScalingCylinderCollider::CollidesWithPlayer(FPPlayerCollider *other) {
    glm::vec3 sphere_position = other->GetPlayer().transform.GetPosition();
    glm::vec3 cylinder_position = owner_.transform.GetPosition();
    float height = owner_.transform.GetScale().y;
    float radius = owner_.transform.GetScale().x * 0.5f;
    float closestPointOnAxis = glm::clamp(sphere_position.y, cylinder_position.y, cylinder_position.y + height);
    float distanceSquared = glm::distance2(glm::vec3(sphere_position.x, closestPointOnAxis, sphere_position.z), cylinder_position);
    return distanceSquared <= (radius + other->GetRadius()) * (radius + other->GetRadius());
}
bool ScalingCylinderCollider::CollidesWithSphere(SphereCollider *other) {
    glm::vec3 sphere_position = other->GetOwner().transform.GetPosition();
    glm::vec3 cylinder_position = owner_.transform.GetPosition();
    float height = owner_.transform.GetScale().y;
    float radius = owner_.transform.GetScale().x * 0.5f;
    float closestPointOnAxis = glm::clamp(sphere_position.y, cylinder_position.y - (height * 0.5f), cylinder_position.y + (height * 0.5f));
    float distanceSquared = glm::distance2(glm::vec3(cylinder_position.x, closestPointOnAxis, cylinder_position.z), sphere_position);
    return distanceSquared <= (radius + other->GetRadius()) * (radius + other->GetRadius());
}


