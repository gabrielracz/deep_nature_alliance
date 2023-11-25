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
        Item(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "")
        : SceneNode(name, mesh_id, shader_id, texture_id) {};

        ~Item();
        
        void SetCollectCallback(std::function<void()> f) { callback = f; };
        virtual void Update(double dt) override;
        
    private:
        std::function<void()> callback;
}; 

#endif 
