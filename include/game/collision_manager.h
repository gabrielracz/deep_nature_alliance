#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_

#include "scene_node.h"
#include "collidable.h"
#include "player.h"

class Game;

class CollisionManager{

    public:
        CollisionManager(){}
        // CollisionManager(const std::vector<Trigger *>& t, const std::vector<Collidable*>& a, Player* p)
        //     : triggers(t), asteroids(a), player(p) {}

        void CheckCollisions();

        void AddNode(Collidable* node);

        // void AddTrigger(Trigger *newTrigger) { triggers.push_back(newTrigger); }

        // void AddAsteroid(Collidable *newAsteroid) { asteroids.push_back(newAsteroid); }

        // void SetPlayer(Player *newPlayer){ player = newPlayer; }

    private:

        bool sphereToSphere(Collidable *first, Collidable *second);
        bool sphereToBox(Collidable *sphereNode, Collidable *boxNode);
        bool rayToSphere(Collidable *rayNode, Collidable *sphereNode);

        std::vector<Collidable*> things; 
};

#endif // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_COLLISION_MANAGER_H_