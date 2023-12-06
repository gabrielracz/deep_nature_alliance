#ifndef COLLIDERS_H
#define COLLIDERS_H

#include <glm/glm.hpp>
#include "terrain.h"
#include "fp_player.h"
#include "scene_node.h"
#include <optional>

class Collider;
class BoxCollider;
class SphereCollider;
class TerrainCollider;
class FPPlayerCollider;
class CylinderCollider;
class ScalingCylinderCollider;

class Collider
{
public:
    using CallbackNoCollider = std::function<void()>;
    using CallbackCollider = std::function<void(SceneNode*)>;
    virtual ~Collider() = default;
    virtual bool CollidesWith(Collider *other) = 0;
    virtual bool CollidesWithBox(BoxCollider *other) { return false; }
    virtual bool CollidesWithSphere(SphereCollider *other) { return false; }
    virtual bool CollidesWithTerrain(TerrainCollider *other) { return false; }
    virtual bool CollidesWithPlayer(FPPlayerCollider *other) { return false; }
    virtual bool CollidesWithCylinder(CylinderCollider *other) { return false; }
    virtual bool CollidesWithScalingCylinder(ScalingCylinderCollider *other) { return false; }
    void SetCallback(CallbackCollider f) { callback_col = std::move(f); };
    void SetCallback(CallbackNoCollider f) { callback_no = std::move(f); };
    void invokeCallback(SceneNode* collider) { 
        if(callback_col) {
            callback_col(collider);
        } else if(callback_no) {
            callback_no();
        }
     };
    void invokeCallback() { if(callback_no) callback_no(); };
private:
    CallbackNoCollider callback_no;
    CallbackCollider callback_col;
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
    bool scaled_;

public:
    SphereCollider(SceneNode& node, float radius, bool scaled = false)
        : owner_(node), radius_(radius), scaled_(scaled) {}
    float GetRadius() const { return scaled_ ?  owner_.transform.GetScale().x : radius_; }
    bool GetScaled() const { return scaled_; }
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
    bool CollidesWithCylinder(CylinderCollider *other) override;
    bool CollidesWithScalingCylinder(ScalingCylinderCollider *other) override;
};

class CylinderCollider : public Collider
{
private:
    SceneNode& owner_;
    float height_;
    float radius_;

public:
    CylinderCollider(SceneNode& node, float radius, float height) : owner_(node), radius_(radius), height_(height) {}
    float GetHeight() const { return height_; }
    float GetRadius() const { return radius_; }
    SceneNode& GetOwner() const { return owner_; }
    bool CollidesWith(Collider *other) override { return other->CollidesWithCylinder(this); }
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
};

class ScalingCylinderCollider : public Collider
{
private:
    SceneNode& owner_;

public:
    ScalingCylinderCollider(SceneNode& node) : owner_(node) {}
    SceneNode& GetOwner() const { return owner_; }
    bool CollidesWith(Collider *other) override { return other->CollidesWithScalingCylinder(this); }
    bool CollidesWithPlayer(FPPlayerCollider *other) override;
    bool CollidesWithSphere(SphereCollider *other) override;
};

#endif // COLLIDERS_H