#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_ENGINE_COLLIDABLE_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_ENGINE_COLLIDABLE_H_

#include <functional>

#include "collision_data.h"
#include "scene_node.h"

class Collidable : public SceneNode {
    public:
        Collidable(
            const std::string name,const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id)
            : SceneNode(name, mesh_id, shader_id, texture_id), collision() {}

        // Collidable(
        //     const std::string name, const std::string &mesh_id, const std::string &shader_id, const std::string &texture_id,
        //     std::function<void(Collidable *)> cAct = [](Collidable *) {}, // Default collisionAction does nothing
        //     std::function<void()> act = []() {}                           // Default action does nothing
        //     ) : SceneNode(name, mesh_id, shader_id, texture_id), collision()
        // {}

        // virtual void Update(double dt) override;
        virtual void Collide(Collidable* other);

        void SetCollision(const CollisionData& t) { collision = t; }
        const CollisionData& GetCollision() const { return collision; }

    protected:
        glm::vec3 closestPossiblePos(Collidable*other);

    private:
        CollisionData collision;
};

#endif  // __DEEP_NATURE_ALLIANCE_INCLUDE_ENGINE_COLLIDABLE_H_