#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include "random.h"
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
#include "texture.h"
#include "shader.h"
#include "defines.h"

// Default extensions for different shader source files
#define VERTEX_PROGRAM_EXTENSION "_vp.glsl"
#define FRAGMENT_PROGRAM_EXTENSION "_fp.glsl"

// Class that manages all resources
class ResourceManager {

    public:
        ResourceManager() = default;
        ~ResourceManager() = default;

        Mesh* GetMesh(const std::string& name);
        Shader* GetShader(const std::string& name);
        Texture* GetTexture(const std::string& name);

        void LoadShader(const std::string& name, const std::string& vert_path, const std::string& frag_path, bool instaced = false);
        void LoadMesh(const std::string& name, const std::string& path);
        void AddMesh(const std::string& name, std::vector<float> verts, std::vector<unsigned int> inds, Layout layout);
        void LoadTexture(const std::string& name, const std::string& path, int wrap_option = GL_REPEAT, int sample_option = GL_NEAREST);

        void CreateTorus(std::string object_name, float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30);
        void CreateSphere(std::string object_name, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45);
        void CreateCylinder(std::string object_name, float height = 1.0, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45);
        void CreateCone(std::string object_name, float height = 1.0, float radius = 0.6, int num_samples_theta = 90, int num_samples_phi = 45, glm::vec4 color = glm::vec4(0.0f));
        void CreateTree(std::string object_name);
        void CreatePointCloud(std::string object_name, int num_points, float size, glm::vec4 color = {0.0f, 0.0f, 0.0f, 0.0f});
        void CreateFire(std::string object_name, int num_points, float size, glm::vec4 color = {0.0f, 0.0f, 0.0f, 0.0f});
        void CreateQuad(std::string object_name);

        
    private:
        RandGenerator rng;
        std::unordered_map<std::string, Mesh>         meshes;
        std::unordered_map<std::string, Shader>       shaders;
        std::unordered_map<std::string, Texture> textures;
        // std::unordered_map<std::string, Sound>     sounds;

        std::string LoadTextFile(const char *filename);


}; // class ResourceManager

#endif // RESOURCE_MANAGER_H_
