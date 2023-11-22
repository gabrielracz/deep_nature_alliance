#ifndef COLLIDERS_H
#define COLLIDERS_H

#include <glm/glm.hpp>
#include "terrain.h"
#include "fp_player.h"
#include "scene_node.h"

class Collider;
class BoxCollider;
class SphereCollider;
class TerrainCollider;
class FPPlayerCollider;

class Collider
{
public:
    virtual ~Collider() = default;
    virtual bool CollidesWith(Collider *other) = 0;
    virtual bool CollidesWithBox(BoxCollider *other) = 0;
    virtual bool CollidesWithSphere(SphereCollider *other) = 0;
    virtual bool CollidesWithTerrain(TerrainCollider *other) = 0;
    virtual bool CollidesWithPlayer(FPPlayerCollider *other) = 0;
};

class BoxCollider : public Collider
{
private:
    SceneNode& owner_;
    glm::vec3 box_half_sizes_;

    bool SphereCollision(SceneNode& collider, float radius);

public:
    BoxCollider(SceneNode& node, float radius)
        : owner_(node) {}

    glm::vec3 GetHalfSizes() const { return box_half_sizes_; }
    SceneNode& GetOwner() const { return owner_; }
    bool CollidesWith(Collider *other) override { return other->CollidesWithBox(this); };
    bool CollidesWithBox(BoxCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
    bool CollidesWithTerrain(TerrainCollider *other) override;
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
};

class SphereCollider : public Collider
{
private:
    SceneNode& owner_;
    float radius_;

public:
    SphereCollider(SceneNode& node, float radius)
        : owner_(node), radius_(radius) {}
    
    float GetRadius() const { return radius_; }
    SceneNode& GetOwner() const { return owner_; }
    bool CollidesWith(Collider *other) override { return other->CollidesWithSphere(this); }
    bool CollidesWithBox(BoxCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
    bool CollidesWithTerrain(TerrainCollider *other) override;
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
};

class TerrainCollider : public Collider
{
private:
    Terrain& t_;
public:
    TerrainCollider(Terrain& terrain)
        : t_(terrain) {}
        
    Terrain& GetTerrain() const { return t_; }
    bool CollidesWith(Collider *other) override { return other->CollidesWithTerrain(this); }
    bool CollidesWithBox(BoxCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
    bool CollidesWithTerrain(TerrainCollider *other) override;
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
};

class FPPlayerCollider : public Collider
{
private:
    FP_Player& player_;
    float radius_;

public:
    FPPlayerCollider(FP_Player& player, float radius)
        : player_(player), radius_(radius) {}

    float GetRadius() const { return radius_; }
    FP_Player& GetPlayer() const { return player_; }
    bool CollidesWith(Collider *other) override { return other->CollidesWithPlayer(this); }
    bool CollidesWithBox(BoxCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
    bool CollidesWithTerrain(TerrainCollider *other) override;
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
};

#endif // COLLIDERS_H