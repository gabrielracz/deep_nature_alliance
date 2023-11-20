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
#include "collision_data.h"
#include "defines.h"

// Class that manages one object in a scene 
class SceneNode {

    struct MaterialProperties {
        float texture_repetition = 1.0f;
        float normal_map_repetition = 1.0f;
        float specular_power = 69.0f;
        float diffuse_strength  = 0.8;
    };

    public:
        // Create scene node from given resources
        SceneNode(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id = "")
        : name(name), mesh_id(mesh_id), shader_id(shader_id), texture_id(texture_id), collision() {};

        // Destructor
        ~SceneNode();
        
        virtual void Update(double dt);

        virtual void SetUniforms(Shader* shader, const glm::mat4& view_matrix);

        void SetTexture(std::string& new_tex_id, float texture_repetition = 1.0f);
        void SetNormalMap(const std::string& new_tex_id, float normal_map_repetition = 1.0f); 
        void AddChild(SceneNode* n)                         {children.push_back(n);}
        void SetCollision(const CollisionData& t)           {collision = t;}
        void AddInstance(Transform t)                      {instances.push_back(t);};
        // void SetInstances(std::vector<Transform>& t)        {instances = t;};

        const std::string GetName(void) const               {return name;}
        const std::string GetMeshID() const                 {return mesh_id;}
        const std::string GetShaderID() const               {return shader_id;}
        const std::string GetTextureID() const              {return texture_id;}
        const std::string GetNormalMap() const              {return normalmap_id;}
        Camera::Projection GetDesiredProjection() const     {return camera_projection;}
        bool IsAlphaEnabled() const                         {return alpha_enabled;}
        const glm::mat4& GetCachedTransformMatrix() const   {return transf_matrix;}
        const std::vector<SceneNode*>& GetChildren() const  {return children;}
        const CollisionData& GetCollision() const           {return collision;}
        std::vector<Transform>& GetInstances()              {return instances;}

        Transform transform;
        MaterialProperties material;
        bool active = true;
        bool visible = true;
        int inverted = 0;
        RenderMode render_mode = FILL;
    protected:
        glm::mat4 transf_matrix;
        std::string name; // Name of the scene node
        std::vector<SceneNode*> children;
        std::vector<Transform> instances;
        double elapsed = 0;
        CollisionData collision;

        std::string mesh_id;
        std::string shader_id;
        std::string texture_id;
        std::string normalmap_id = "";

        Camera::Projection camera_projection = Camera::Projection::PERSPECTIVE;
        bool alpha_enabled = false;
}; // class SceneNode

#endif // SCENE_NODE_H_
