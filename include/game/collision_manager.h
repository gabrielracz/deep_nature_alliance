#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_

#include <memory>

#include "scene_node.h"
#include "trigger.h"
#include "player.h"
#include "item.h"
#include "asteroid.h"
#include "toggle.h"

class Game;

class CollisionManager{

    public:
        CollisionManager(Game* game) : triggers(), items(), asteroids(), beacons(), othercollideables(), game(game) {}
        // CollisionManager(const std::vector<Trigger *>& t, const std::vector<SceneNode*>& a, Player* p)
        //     : triggers(t), asteroids(a), player(p) {}

        void CheckCollisions();

        void AddNode(std::shared_ptr<SceneNode> node);

        void Reset();

        // void AddTrigger(Trigger *newTrigger) { triggers.push_back(newTrigger); }

        // void AddAsteroid(SceneNode *newAsteroid) { asteroids.push_back(newAsteroid); }

        void SetPlayer(std::shared_ptr<Player> newPlayer) { player = std::move(newPlayer); }

    private:

        bool sphereToSphere(SceneNode& first, SceneNode& second);
        bool sphereToBox(SceneNode& sphereNode, SceneNode& boxNode);
        bool rayToSphere(SceneNode& rayNode, SceneNode& sphereNode);

        bool GetCollision(SceneNode& obj1, SceneNode& obj2);
        bool GetCollisionRaw(SceneNode& obj1, SceneNode&  obj2);

        std::vector<std::shared_ptr<Trigger>> triggers;
        std::vector<std::shared_ptr<Toggle>> toggles;
        std::vector<std::shared_ptr<SceneNode>> items;
        std::vector<std::shared_ptr<SceneNode>> asteroids;
        std::vector<std::shared_ptr<SceneNode>> beacons;
        std::vector<std::shared_ptr<SceneNode>> othercollideables;
        std::vector<std::shared_ptr<SceneNode>> rockets;
        std::vector<std::shared_ptr<SceneNode>> blockingCollision;

        std::shared_ptr<Player> player;
        Game* game = nullptr;
        
};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_