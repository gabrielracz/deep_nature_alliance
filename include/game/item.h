#ifndef ITEM_H_
#define ITEM_H_

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

class Item : public SceneNode {

    public:
        Item(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id);

        ~Item();
        
        void SetCollectCallback(std::function<void()> f) { callback = f; };
        virtual void Update(double dt) override;
        virtual void HandleCollisionWith(SceneNode* collider);
        
    private:
        std::function<void()> callback;
        float item_col_radius_ = 3.0f;
        float current_time_ = 0.0f;
        float hover_amplitude_ = 0.005f;
        float hover_speed_ = 2.0f;
        float hover_offset_ = 0.0f;
};

 

#endif 
