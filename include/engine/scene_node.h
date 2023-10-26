#ifndef SCENE_NODE_H_
#define SCENE_NODE_H_

#include <string>
#include <vector>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#include <glm/gtc/quaternion.hpp>

#include "resource.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"

// Class that manages one object in a scene 
class SceneNode {

    public:
        // Create scene node from given resources
        SceneNode(const std::string name,  Mesh* mesh, Shader* shader);

        // Destructor
        ~SceneNode();
        
        // Get name of node
        const std::string GetName(void) const;

        // Get node attributes
        glm::vec3 GetPosition(void) const;
        glm::quat GetOrientation(void) const;
        glm::vec3 GetScale(void) const;

        // Set node attributes
        void SetPosition(glm::vec3 position);
        void SetOrientation(glm::quat orientation);
        void SetScale(glm::vec3 scale);
        
        // Perform transformations on node
        // void Translate(glm::vec3 trans);
        // void Rotate(glm::quat rot);
        // void Scale(glm::vec3 scale);

        // Draw the node according to scene parameters in 'camera'
        // variable
        virtual void Draw(Camera* camera, const glm::mat4& parent_transform = glm::mat4(1.0));

        // Update the node
        virtual void Update(double dt);

        Transform transform;
        int inverted = 0;
        bool active = true;
        bool visible = true;
        double elapsed = 0;

        std::vector<SceneNode*> children;
    private:
        glm::mat4 transf_matrix;
        std::string name_; // Name of the scene node

        Mesh* mesh       {nullptr};
        Shader* shader   {nullptr};
        Texture* texture {nullptr};

        // Set matrices that transform the node in a shader program
        void SetUniforms(Shader* shader, const glm::mat4& parent_matrix);

}; // class SceneNode

#endif // SCENE_NODE_H_
