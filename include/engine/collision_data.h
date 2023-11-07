#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_ENGINE_COLLISION_DATA_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_ENGINE_COLLISION_DATA_H_

#include <glm/glm.hpp>

enum CollisionType {
    SPHERE,
    BOX,
    RAY
};

class CollisionData {
public:
    CollisionData() 
        : CollisionData(5.0) {}

    CollisionData(float radius) 
        : type(CollisionType::SPHERE), sphereRadius(radius), collisionEnabled(true) {}

    CollisionData(const glm::vec3& halfSizes) 
        : type(CollisionType::BOX), boxHalfSizes(halfSizes), collisionEnabled(true) {}

    CollisionData(const glm::vec3& origin, const glm::vec3& direction) 
        : type(CollisionType::RAY), rayOrigin(origin), rayDirection(glm::normalize(direction)), collisionEnabled(true) {}

    CollisionType GetType() const { return type; }
    float GetSphereRadius() const { return collisionEnabled ? sphereRadius : 0.0f; }
    const glm::vec3& GetBoxHalfSizes() const { static const glm::vec3 zeroVec(0.0f); return collisionEnabled ? boxHalfSizes : zeroVec; }
    const glm::vec3& GetRayOrigin() const { static const glm::vec3 zeroVec(0.0f); return collisionEnabled ? rayOrigin : zeroVec; }
    const glm::vec3& GetRayDirection() const { static const glm::vec3 zeroVec(0.0f); return collisionEnabled ? rayDirection : zeroVec; }

    void SetSphereRadius(float radius)                  { sphereRadius = radius; }
    void SetBoxHalfSizes(const glm::vec3& halfSizes)    { boxHalfSizes = halfSizes; }
    void SetRayOrigin(const glm::vec3& origin)          { rayOrigin = origin; }
    void SetRayDirection(const glm::vec3& direction)    { rayDirection = glm::normalize(direction); }

    bool IsCollisionEnabled() const { return collisionEnabled; }
    void SetCollisionEnabled(bool enabled) { collisionEnabled = enabled; }

private:
    CollisionType type;
    bool collisionEnabled;
    // Sphere properties
    float sphereRadius; // Only radius is needed, position will come from SceneNode's transform
    // Box properties
    glm::vec3 boxHalfSizes; // Half sizes of the box, position will come from SceneNode's transform
    // Ray properties
    glm::vec3 rayOrigin;
    glm::vec3 rayDirection; 
};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_ENGINE_COLLISION_DATA_H_
