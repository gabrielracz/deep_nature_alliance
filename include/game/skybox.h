#ifndef __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_SKYBOX_H_
#define __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_SKYBOX_H_

#include "scene_node.h"
#include "player.h"

class Skybox : public SceneNode {

    public:
        Skybox(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id, Player* player = nullptr)
            : SceneNode(name, mesh_id, shader_id, texture_id), track(player) {}

        virtual void Update(double dt) override {
            if (track){
                transform.SetPosition(track->transform.GetPosition());
            }
        }

    private:
        Player *track;

};

#endif  // __DEEP_NATURE_ALLIANCE_INCLUDE_GAME_SKYBOX_H_