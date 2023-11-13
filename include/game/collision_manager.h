#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_

#include "scene_node.h"
#include "trigger.h"
#include "player.h"

class Game;

class CollisionManager{

    public:
        CollisionManager() : triggers(), asteroids(){}
        // CollisionManager(const std::vector<Trigger *>& t, const std::vector<SceneNode*>& a, Player* p)
        //     : triggers(t), asteroids(a), player(p) {}

        void CheckCollisions();

        void AddTrigger(Trigger *newTrigger) { triggers.push_back(newTrigger); }

        void AddAsteroid(SceneNode *newAsteroid) { asteroids.push_back(newAsteroid); }

        void SetPlayer(Player *newPlayer){ player = newPlayer; }

    private:

        bool sphereToSphere(SceneNode *first, SceneNode *second);
        bool sphereToBox(SceneNode *sphereNode, SceneNode *boxNode);
        bool rayToSphere(SceneNode *rayNode, SceneNode *sphereNode);

        std::vector<Trigger *> triggers;
        std::vector<SceneNode*> asteroids;
        Player *player;
        
};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_