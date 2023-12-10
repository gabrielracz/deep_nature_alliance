#ifndef MEYE_H_
#define MEYE_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/random.hpp>
#include "scene_node.h"

class MoonEye : public SceneNode {

    public:
        MoonEye(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id);
        
        virtual void Update(double dt) override;
    private:
        float current_time_ = 0.0f;
        float a1_;
        float a2_;
        float a3_;
        float radius_;
        float rot_speed_;
        float orbit_speed_;
};

 

#endif 
