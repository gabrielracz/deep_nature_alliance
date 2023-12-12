#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_

#include "scene_node.h"
#include "trigger.h"
#include "player.h"
#include "item.h"
#include "asteroid.h"

class Game;

class CollisionManager{

    public:
        CollisionManager(Game* game) : triggers(), items(), asteroids(), beacons(), othercollideables(), game(game) {}
        // CollisionManager(const std::vector<Trigger *>& t, const std::vector<SceneNode*>& a, Player* p)
        //     : triggers(t), asteroids(a), player(p) {}

        void CheckCollisions();

        void AddNode(SceneNode* node);

        void Reset();

        // void AddTrigger(Trigger *newTrigger) { triggers.push_back(newTrigger); }

        // void AddAsteroid(SceneNode *newAsteroid) { asteroids.push_back(newAsteroid); }

        void SetPlayer(Player *newPlayer){ player = newPlayer; }

    private:

        bool sphereToSphere(SceneNode *first, SceneNode *second);
        bool sphereToBox(SceneNode *sphereNode, SceneNode *boxNode);
        bool rayToSphere(SceneNode *rayNode, SceneNode *sphereNode);

        void GetCollision(SceneNode* obj1, SceneNode* obj2);

        std::vector<Trigger *> triggers;
        std::vector<SceneNode*> items;
        std::vector<SceneNode*> asteroids;
        std::vector<SceneNode*> beacons;
        std::vector<SceneNode*> othercollideables;
        SceneNode *player;
        Game* game = nullptr;
        
};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_