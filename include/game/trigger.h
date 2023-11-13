#ifndef TRIGGER_H_
#define TRIGGER_H_

#include <functional>
#include "scene_node.h"

class Game;

class Trigger : public SceneNode {

    public:
        Trigger(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id, std::function<void()> act)
        : SceneNode(name, mesh_id, shader_id, texture_id), action(act) {}
        // Trigger(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id, Game* g,std::function<void()> act)
        // : SceneNode(name, mesh_id, shader_id, texture_id), action(act), game(g) {}
        
        virtual void Update(double dt) override;
        void SetAction(std::function<void()> act) { action = act;}
        void ActivateTrigger();

    private:
        std::function<void()> action;
        Game* game;
};

#endif // TRIGGER_H_
