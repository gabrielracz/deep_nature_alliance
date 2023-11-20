#ifndef TERRAIN_H
#define TERRAIN_H
#include "scene_node.h"
#include "glm/gtc/random.hpp"

class Game;

enum class TerrainType {
    MOON,
    FOREST,
    LAVA
};

class Terrain : public SceneNode {
    public:

        Terrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id, TerrainType type, float xwidth, float zwidth, float density, Game* game);

        float SampleHeight(float x, float z);
        float SampleSlope(float x, float z, glm::vec3 dir = {0.0f, 0.0f, 0.0f});
        bool SamplePassable(float x, float z);
        float SampleAngle(float x, float z, glm::vec3 dir);
        glm::vec3 SampleNormal(float x, float z);

        bool IsDeathTerrain() const { return death_terrain_; }

    protected:
        void GenerateHeightmap(TerrainType type);
        void GenerateQMoon();
        void GenerateForest();
        void GenerateLava();
        void GenerateNormals();
        void GenerateObstacles();
        void GenerateTangents();
        void GenerateUV();
        void GenerateMesh();
        void GenerateImPassable();

        glm::vec2 IndexGrid(float x, float z);
        glm::vec3 InterpNormals(int x0, int z0, float sx, float sz);

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        std::vector<std::vector<float>> heights;
        std::vector<std::vector<glm::vec3>> normals;
        std::vector<std::vector<glm::vec3>> tangents;
        std::vector<std::vector<bool>> obstacles;
        std::vector<std::vector<bool>> impassable;
        std::vector<std::vector<glm::vec2>> uvs;

        float xwidth;
        float zwidth;
        float num_xsteps;
        float num_zsteps;
        float xstep;
        float zstep;
        float density;
        
        bool death_terrain_;

        TerrainType type;

        Game* game;
};
#endif