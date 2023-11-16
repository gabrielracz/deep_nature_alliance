#ifndef TERRAIN_H
#define TERRAIN_H
#include "scene_node.h"
class Game;

class Terrain : public SceneNode {
    public:
        Terrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, float xwidth, float zwidth, float density, Game* game);

        float SampleHeight(float x, float z);
        float SampleSlope(float x, float z, glm::vec3 dir = {0.0f, 0.0f, 0.0f});

    private:
        void GenerateHeightmap();
        void GenerateNormals();
        void GenerateObstacles();
        void GenerateTangents();
        void GenerateMesh();

        glm::vec2 IndexGrid(float x, float z);
        glm::vec3 InterpNormals(int x0, int z0, float sx, float sz);

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        std::vector<std::vector<float>> heights;
        std::vector<std::vector<glm::vec3>> normals;
        std::vector<std::vector<glm::vec3>> tangents;
        std::vector<std::vector<bool>> obstacles;

        float xwidth;
        float zwidth;
        float num_xsteps;
        float num_zsteps;
        float xstep;
        float zstep;

        Game* game;
};

#endif