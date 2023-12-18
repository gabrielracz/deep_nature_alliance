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
#include "node_types.h"

class Collider;

// Class that manages one object in a scene 
class SceneNode {


    struct MaterialProperties {
        float texture_repetition = 1.0f;
        float normal_map_repetition = 1.0f;
        float specular_power = 69.0f;
        float diffuse_strength  = 0.8f;
        float ambient_additive = 0.0f;
        float specular_coefficient = 1.0f;
    };

    public:
        // Create scene node from given resources
        SceneNode(const std::string name, const std::string& mesh_id, const std::string& shader_id, const std::string& texture_id = "");

        // Destructor
        ~SceneNode();
        
        virtual void Update(double dt);

        virtual void SetUniforms(Shader* shader, const glm::mat4& view_matrix, const glm::mat4& parent_matrix = glm::mat4(1.0f));

        void SetTexture(std::string& new_tex_id, float texture_repetition = 1.0f);
        void SetNormalMap(const std::string& new_tex_id, float normal_map_repetition = 1.0f); 
        void AddChild(SceneNode* n);                        
        void SetCollision(const CollisionData& t)           {collision = t;}
        void SetCollider(Collider * col)                    {collider = col;}
        void SetNodeType(NodeType type)                     {node_type = type;}
        void AddInstance(Transform t)                       {instances.push_back(t);};
        void SetAlphaEnabled(bool a)                        {alpha_enabled = a;}
        void SetAlphaFunc(int f)                            {alpha_func = f;}
        void SetParent(SceneNode* n)                        {parent = n;}
        void DeleteInstance(unsigned int i)                 {deleted_instances.push_back(i);}
        void SetCullInstances(bool c)                       {cull_instances = c;}
        // void SetInstances(std::vector<Transform>& t)        {instances = t;};

        const std::string GetName(void) const               {return name;}
        const std::string GetMeshID() const                 {return mesh_id;}
        const std::string GetShaderID() const               {return shader_id;}
        const std::string GetTextureID() const              {return texture_id;}
        const std::string GetNormalMap() const              {return normalmap_id;}
        Camera::Projection GetDesiredProjection() const     {return camera_projection;}
        bool IsAlphaEnabled() const                         {return alpha_enabled;}
        int GetAlphaFunc() const                            {return alpha_func;}
        const glm::mat4& GetCachedTransformMatrix() const   {return transf_matrix;}
        const std::vector<SceneNode*>& GetChildren() const  {return children;}
        const CollisionData& GetCollision() const           {return collision;}
        std::vector<Transform>& GetInstances()              {return instances;}
        bool ShouldCullInstances()                          {return cull_instances;}
        int GetNumInstances()                               {return in_camera_instances;}
        Collider* GetCollider() const                       {return collider;}
        NodeType GetNodeType() const                        {return node_type;}

        virtual void HandleCollisionWith(SceneNode* collider) {};

        Transform transform;
        MaterialProperties material;
        bool active = true;
        bool visible = true;
        bool deleted = false;
        int inverted = 0;
        RenderMode render_mode = FILL;
    protected:
        glm::mat4 transf_matrix;
        std::string name; // Name of the scene node
        std::vector<SceneNode*> children;
        SceneNode* parent = nullptr;
        std::vector<Transform> instances;
        std::vector<unsigned int> deleted_instances;
        bool cull_instances = false;
        int in_camera_instances = 0;
        float elapsed = 0;
        CollisionData collision;

        std::string mesh_id = "";
        std::string shader_id = "";
        std::string texture_id = "";
        std::string normalmap_id = "";

        Camera::Projection camera_projection = Camera::Projection::PERSPECTIVE;
        bool alpha_enabled = false;
        int alpha_func = GL_ONE_MINUS_SRC_ALPHA;

        NodeType node_type = TNODE;
        Collider* collider = nullptr;
}; // class SceneNode

#endif // SCENE_NODE_H_
