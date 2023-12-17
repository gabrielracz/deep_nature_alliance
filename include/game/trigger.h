#ifndef TRIGGER_H_
#define TRIGGER_H_

#include <functional>
#include "scene_node.h"
#include "shader.h"

class Trigger : public SceneNode {
    public:
        Trigger(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id, std::function<void()> act = nullptr)
            : SceneNode(name, mesh_id, shader_id, texture_id), action(act), timerActive(false), shd(nullptr) {
            SetNodeType(TTRIGGER);
        }
        // Trigger(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id, Game* g,std::function<void()> act)
        // : SceneNode(name, mesh_id, shader_id, texture_id), action(act), game(g) {}

        virtual void Update(double dt) override;
        void SetAction(std::function<void()> act) { action = act; }
        void SetShader();
        void ActivateTrigger();

        void SuperJankDesertHeatTrigger();

        void SetTimer(double duration) { timerDuration = duration; }
        void StartTimer() {
            elapsedTime = 0.0;
            timerActive = true;
        }
        void StopTimer() { timerActive = false; }
        void ResetTimer() { elapsedTime = 0.0; }

    private:
        std::function<void()> action;

        Shader * shd;

        double timerDuration;
        double elapsedTime;
        bool timerActive;
};

#endif  // TRIGGER_H_
