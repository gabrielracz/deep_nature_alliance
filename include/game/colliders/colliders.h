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
    SceneNode& node_;

public:
    BoxCollider(SceneNode& node, float radius)
        : node_(node) {}

    bool CollidesWith(Collider *other) override;
    bool CollidesWithBox(BoxCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
    bool CollidesWithTerrain(TerrainCollider *other) override;
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
};

class SphereCollider : public Collider
{
private:
    SceneNode& node_;
    float radius_;

public:
    SphereCollider(SceneNode& node, float radius)
        : node_(node), radius_(radius) {}
    
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

public:
    FPPlayerCollider(FP_Player& player)
        : player_(player) {}

    FP_Player& GetPlayer() const { return player_; }
    bool CollidesWith(Collider *other) override { return other->CollidesWithPlayer(this); }
    bool CollidesWithBox(BoxCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
    bool CollidesWithTerrain(TerrainCollider *other) override;
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
};

#endif // COLLIDERS_H