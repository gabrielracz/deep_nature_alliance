#include <glm/gtc/noise.hpp>

#include "terrain.h"
#include "defines.h"
#include "game.h"

Terrain::Terrain(const std::string name, const std::string& mesh_id, const std::string shader_id, const std::string& texture_id,float xwidth, float zwidth, float density, Game* game)
: SceneNode(name, mesh_id, shader_id) {

    Layout layout({
        {FLOAT3, "vertex"},
        {FLOAT3, "normal"},
        {FLOAT3, "color"},
        {FLOAT2, "uv"}
    });

    //generate uniform grid

    int num_xsteps = xwidth*density;
    int num_zsteps = zwidth*density;
    float xstep = xwidth/num_xsteps;
    float zstep = zwidth/num_zsteps;

    for(int z = 0; z < num_zsteps; z++) {
        for(int x = 0; x < num_xsteps; x++) {
            // glm::vec2 sample = glm::vec2(x*xstep, z*zstep) / 100.0f;
            // float height = glm::perlin(sample) * 50.0;
            float height = 0.0;
            //create this vertex
            glm::vec3 pos    = {x*xstep, height, z*zstep};
            glm::vec3 normal = {0.0, 1.0, 0.0};
            glm::vec3 color  = {Colors::SeaBlue};
            glm::vec2 uv     = {0.0, 0.0};

            APPEND_VEC3(vertices, pos);
            APPEND_VEC3(vertices, normal);
            APPEND_VEC3(vertices, color);
            APPEND_VEC2(vertices, uv);
        }
    }


    // triangulate the grid of points
    #define GIX(x, z) ((x) + (z)*num_xsteps)
    for(int z = 0; z < num_zsteps-1; z++) {
        for(int x = 0; x < num_xsteps-1; x++) {
            indices.push_back(GIX(x  , z));
            indices.push_back(GIX(x  , z+1));
            indices.push_back(GIX(x+1, z+1));

            indices.push_back(GIX(x  , z));
            indices.push_back(GIX(x+1, z));
            indices.push_back(GIX(x+1, z+1));
        }
    }

    game->resman.AddMesh(mesh_id, vertices, indices, layout);
}


std::vector<float> Terrain::GenerateHeightmap(float xwidth, float zwidth, float density) {
    return std::vector<float>();
}