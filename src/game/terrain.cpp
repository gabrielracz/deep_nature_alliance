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

    xwidth_ = xwidth;
    zwidth_ = zwidth;
    density_ = density;
    xsteps = num_xsteps;
    zsteps = num_zsteps;

    heights.resize(num_xsteps, std::vector<float>(num_zsteps, 0.0));

    for(int z = 0; z < num_zsteps; z++) {
        for(int x = 0; x < num_xsteps; x++) {
            glm::vec2 sample = glm::vec2(x*xstep, z*zstep) / 100.0f;
            float height = glm::perlin(sample) * 50.0;
            // float height = 0.0;

            heights[x][z] = height;
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

float Terrain::SampleHeight(float x, float z) {
    float terrainX = x / (xwidth_ / (heights.size() - 1)) + (xsteps / 2.0);
    float terrainZ = z / (zwidth_ / (heights[0].size() - 1)) + (zsteps / 2.0);

    // Get the integer coordinates of the cell
    int x0 = (int)(std::floor(terrainX));
    int z0 = (int)(std::floor(terrainZ));

    // Clamp the coordinates to be within valid range
    x0 = std::clamp(x0, 0, (int)(heights.size()) - 2);
    z0 = std::clamp(z0, 0, (int)(heights[0].size()) - 2);

    // Get the fractional part of the coordinates
    float sx = terrainX - x0;
    float sz = terrainZ - z0;

    // Perform bilinear interpolation
    float h00 = heights[x0][z0];
    float h10 = heights[x0 + 1][z0];
    float h01 = heights[x0][z0 + 1];
    float h11 = heights[x0 + 1][z0 + 1];

    float h0 = (1 - sx) * h00 + sx * h10;
    float h1 = (1 - sx) * h01 + sx * h11;

    return (1 - sz) * h0 + sz * h1;
}
