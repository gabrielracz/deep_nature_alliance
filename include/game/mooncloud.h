#ifndef MOONCLOUD_H_
#define MOONCLOUD_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include "scene_node.h"

class MoonCloud : public SceneNode {

    public:
        MoonCloud(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id);
        
        virtual void Update(double dt) override;
    private:
        float current_time_ = 0.0f;
        float hover_amplitude_ = 0.005f;
        float hover_speed_ = 1.4f;
        float hover_offset_ = 0.0f;
};

 

#endif 
