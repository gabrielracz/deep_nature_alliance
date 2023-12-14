#ifndef TOGGLE_H_
#define TOGGLE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <functional>

#include "resource.h"
#include "scene_node.h"

class Toggle : public SceneNode {

    public:
        Toggle(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id);
        
        void SetOffCallback(std::function<void()> f) { off_callback_ = f; };
        void SetOnCallback(std::function<void()> f) { on_callback_ = f; };
        virtual void ToggleOn(SceneNode* collider);
        virtual void ToggleOff(SceneNode* exiter);
    
        bool GetToggle() { return toggle_; }
        bool Triggered() { return triggered_; }

        void SetMaxTriggerCount(int max) { max_trigger_count_ = max; }
        
    protected:
        std::function<void()> off_callback_;
        std::function<void()> on_callback_;
        bool toggle_ = false;
        bool triggered_ = false;
        int trigger_count_ = 0;
        int max_trigger_count_ = 0;
};

#endif 