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
        
        const std::string GetName(void) const;
        virtual void Update(double dt);

        bool active = true;
        bool visible = true;
        double elapsed = 0;
        int inverted = 0;

        std::vector<SceneNode*> children;
        void SetTexture(Texture* newtex) {texture = newtex;}
        virtual void SetUniforms(Camera& camera, const glm::mat4& parent_matrix);

        Mesh* GetMesh() {return mesh;}
        Shader* GetShader() {return shader;}
        Texture* GetTexture() {return texture;}
        Camera::Projection GetDesiredProjection() {return camera_projection;}
        bool IsAlphaEnabled() {return alpha_enabled;}
        const glm::mat4& GetCachedTransformMatrix(){return transf_matrix;}
        std::vector<SceneNode*> GetChildren() {return children;}

        Transform transform;
    protected:
        glm::mat4 transf_matrix;
        std::string name_; // Name of the scene node

        Mesh* mesh       {nullptr};
        Shader* shader   {nullptr};
        Texture* texture {nullptr};

        Camera::Projection camera_projection = Camera::Projection::PERSPECTIVE;
        bool alpha_enabled = false;

        // Set matrices that transform the node in a shader program

}; // class SceneNode

#endif // SCENE_NODE_H_
