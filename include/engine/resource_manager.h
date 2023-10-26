#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "resource.h"
#include "mesh.h"
#include "shader.h"

// Default extensions for different shader source files
#define VERTEX_PROGRAM_EXTENSION "_vp.glsl"
#define FRAGMENT_PROGRAM_EXTENSION "_fp.glsl"

// Class that manages all resources
class ResourceManager {

    public:
        // Constructor and destructor
        ResourceManager(void);
        ~ResourceManager();
        // Add a resource that was already loaded and allocated to memory
        void AddResource(ResourceType type, const std::string name, GLuint resource, GLsizei size);
        void AddResource(ResourceType type, const std::string name, GLuint array_buffer, GLuint element_array_buffer, GLsizei size);
        // Load a resource from a file, according to the specified type
        void LoadResource(ResourceType type, const std::string name, const char *filename);
        // Get the resource with the specified name
        Resource *GetResource(const std::string name) const;

        Mesh* GetMesh(const std::string& name);
        Shader* GetShader(const std::string& name);
        unsigned int GetTexture(const std::string& name);

        void LoadShader(const std::string& name, const std::string& vert_path, const std::string& frag_path);
        void LoadMesh(const std::string& name, const std::string& path);
        void AddMesh(const std::string& name, std::vector<float> verts, std::vector<unsigned int> inds, Layout layout);
        void LoadTexture(const std::string& name, const std::string& path);

        void CreateTorus(std::string object_name, float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30);
        void CreateSphere(std::string object_name, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45);
        void CreateCylinder(std::string object_name, float height = 1.0, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45);
        void CreateCone(std::string object_name, float height = 1.0, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45, glm::vec4 color = glm::vec4(0.0f));
        void CreateTree(std::string object_name);

        
    private:
        
        // List storing all resources
        std::vector<Resource*> resource_; 

        std::unordered_map<std::string, Mesh>         meshes;
        std::unordered_map<std::string, Shader>       shaders;
        std::unordered_map<std::string, unsigned int> textures;
        // std::unordered_map<std::string, Sound>     sounds;

        // Methods to load specific types of resources
        // Load shaders programs
        void LoadMaterial(const std::string name, const char *prefix);
        // Load a text file into memory (could be source code)
        std::string LoadTextFile(const char *filename);

}; // class ResourceManager

#endif // RESOURCE_MANAGER_H_
