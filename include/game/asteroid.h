#ifndef ASTEROID_H_
#define ASTEROID_H_

#include <string>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "scene_node.h"

// Abstraction of an asteroid
class Asteroid : public SceneNode {

    public:
        // Create asteroid from given resources
        Asteroid(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id = "")
        : SceneNode(name, mesh_id, shader_id, texture_id) {};

        // Destructor
        ~Asteroid();
        
        // Get/set attributes specific to asteroids
        glm::quat GetAngM(void) const;
        void SetAngM(glm::quat angm);

        // Update geometry configuration
        void Update(void);
        
    private:
        // Angular momentum of asteroid
        glm::quat angm_;
}; // class Asteroid

#endif // ASTEROID_H_
